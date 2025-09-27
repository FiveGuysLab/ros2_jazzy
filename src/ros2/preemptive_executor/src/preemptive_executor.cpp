#include "preemptive_executor/preemptive_executor.hpp"

#include <chrono>
#include <functional>
#include <memory>
#include <vector>
#include <sched.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <algorithm>

#include "rcpputils/scope_exit.hpp"
#include "rclcpp/logging.hpp"
#include "rclcpp/utilities.hpp"

using preemptive_executor::PreemptiveExecutor;

// Helper function to set SCHED_DEADLINE parameters
static int set_deadline_scheduling(int runtime_ns, int deadline_ns, int period_ns)
{
    struct sched_attr attr = {};
    attr.size = sizeof(attr);
    attr.sched_policy = SCHED_DEADLINE;
    attr.sched_runtime = runtime_ns;
    attr.sched_deadline = deadline_ns;
    attr.sched_period = period_ns;
    attr.sched_flags = 0;

    int ret = syscall(SYS_sched_setattr, 0, &attr, 0);
    if (ret < 0)
    {
        RCLCPP_ERROR(rclcpp::get_logger("rclcpp"),
                     "Failed to set SCHED_DEADLINE: %s", strerror(errno));
        return -1;
    }
    return 0;
}

PreemptiveExecutor::PreemptiveExecutor(
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

PreemptiveExecutor::~PreemptiveExecutor() {}

void PreemptiveExecutor::spin()
{
    // When the executor is already spinning, throw an error
    if (spinning.exchange(true))
    {
        throw std::runtime_error("spin() called while already spinning");
    }
    // Reset the wait result and store the spinning state
    RCPPUTILS_SCOPE_EXIT(wait_result_.reset(); this->spinning.store(false););

    // TODO : Implement the spin function here
}

size_t
PreemptiveExecutor::get_number_of_threads() const
{
    return number_of_threads_;
}

void PreemptiveExecutor::run(size_t this_thread_number)
{
    (void)this_thread_number;

    while (rclcpp::ok(this->context_) && spinning.load())
    {
        rclcpp::AnyExecutable any_exec;
        {
            // TODO : Implement locks here
        }

        // Execute the callback
        execute_any_executable(any_exec);

        // TODO : Implement mutually exclusive callback groups here

    }
}
