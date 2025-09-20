// Copyright 2024 Open Source Robotics Foundation, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "preemptive_edf_executor/preemptive_edf_executor.hpp"

#include <chrono>
#include <functional>
#include <memory>
#include <vector>

#include "rcpputils/scope_exit.hpp"
#include "rclcpp/logging.hpp"
#include "rclcpp/utilities.hpp"

using preemptive_edf_executor::PreemptiveEDFExecutor;

PreemptiveEDFExecutor::PreemptiveEDFExecutor(
    const rclcpp::ExecutorOptions &options,
    size_t number_of_threads,
    std::chrono::nanoseconds timeout)
    : rclcpp::Executor(options),
      next_exec_timeout_(timeout)
{
    number_of_threads_ = number_of_threads > 0 ? number_of_threads : std::max(std::thread::hardware_concurrency(), 2U);

    if (number_of_threads_ == 1)
    {
        RCLCPP_WARN(
            rclcpp::get_logger("rclcpp"),
            "PreemptiveEDFExecutor is used with a single thread.\n"
            "Use the SingleThreadedExecutor instead.");
    }
}

PreemptiveEDFExecutor::~PreemptiveEDFExecutor() {}

void PreemptiveEDFExecutor::spin()
{
    if (spinning.exchange(true))
    {
        throw std::runtime_error("spin() called while already spinning");
    }
    RCPPUTILS_SCOPE_EXIT(wait_result_.reset(); this->spinning.store(false););

    std::vector<std::thread> threads;
    size_t thread_id = 0;
    {
        std::lock_guard wait_lock{wait_mutex_};
        for (; thread_id < number_of_threads_ - 1; ++thread_id)
        {
            auto func = std::bind(&PreemptiveEDFExecutor::run, this, thread_id);
            threads.emplace_back(func);
        }
    }

    // Run the last thread in the main thread
    run(thread_id);

    // Wait for all other threads to complete
    for (auto &thread : threads)
    {
        thread.join();
    }
}

size_t
PreemptiveEDFExecutor::get_number_of_threads() const
{
    return number_of_threads_;
}

void PreemptiveEDFExecutor::run(size_t this_thread_number)
{
    (void)this_thread_number;

    while (rclcpp::ok(this->context_) && spinning.load())
    {
        rclcpp::AnyExecutable any_exec;
        {
            std::lock_guard wait_lock{wait_mutex_};
            if (!rclcpp::ok(this->context_) || !spinning.load())
            {
                return;
            }

            // Try to get next executable with priority consideration
            if (!get_next_ready_executable(any_exec))
            {
                // If no ready executable, wait for work
                wait_for_work(next_exec_timeout_);
                continue;
            }
        }

        // Execute the callback
        execute_any_executable(any_exec);

        // Handle mutually exclusive callback groups
        if (any_exec.callback_group &&
            any_exec.callback_group->type() == CallbackGroupType::MutuallyExclusive)
        {
            try
            {
                interrupt_guard_condition_->trigger();
            }
            catch (const rclcpp::exceptions::RCLError &ex)
            {
                throw std::runtime_error(
                    std::string(
                        "Failed to trigger guard condition on callback group change: ") +
                    ex.what());
            }
        }

        // Clear the callback_group to prevent the AnyExecutable destructor from
        // resetting the callback group `can_be_taken_from`
        any_exec.callback_group.reset();
    }
}

bool PreemptiveEDFExecutor::get_next_executable(
    rclcpp::AnyExecutable &any_executable,
    std::chrono::nanoseconds timeout)
{
    // For now, use the base class implementation
    // TODO: Implement priority-based selection
    return rclcpp::Executor::get_next_executable(any_executable, timeout);
}

bool PreemptiveEDFExecutor::get_next_ready_executable(
    rclcpp::AnyExecutable &any_executable)
{
    // For now, use the base class implementation
    // TODO: Implement priority and deadline-based selection
    return rclcpp::Executor::get_next_executable(any_executable, std::chrono::nanoseconds(0));
}

void PreemptiveEDFExecutor::wait_for_work(std::chrono::nanoseconds timeout)
{
    // Use the base class wait_for_work implementation
    // TODO: Implement custom waiting logic if needed
    rclcpp::Executor::wait_for_work(timeout);
}
