#ifndef PREEMPTIVE_EDF_EXECUTOR__PREEMPTIVE_EDF_EXECUTOR_HPP_
#define PREEMPTIVE_EDF_EXECUTOR__PREEMPTIVE_EDF_EXECUTOR_HPP_

#include <rmw/rmw.h>

#include <cassert>
#include <cstdlib>
#include <chrono>
#include <memory>
#include <fstream>
#include <mutex>
#include <queue>
#include <thread>
#include <unordered_map>
#include <set>
#include <vector>

#include "rclcpp/executor.hpp"
#include "rclcpp/macros.hpp"
#include "preemptive_executor/preemptive_executor_interface.hpp"
#include "rclcpp/visibility_control.hpp"

namespace preemptive_executor
{

    /// Preemptive-based Earliest Deadline First (EDF) Executor
    /**
     * This executor implements a preemptive-based EDF scheduling algorithm for ROS 2 callbacks.
     * It extends the multi-threaded execution model with priority and deadline awareness.
     */
    class PreemptiveExecutor : public rclcpp::Executor
    {
    public:
        RCLCPP_SMART_PTR_DEFINITIONS(PreemptiveExecutor)

        /// Constructor for PreemptiveEDFExecutor.
        /**
         * \param options common options for all executors
         * \param number_of_threads number of threads to have in the thread pool,
         *   the default 0 will use the number of cpu cores found (minimum of 2)
         * \param timeout maximum time to wait for work
         */
        RCLCPP_PUBLIC
        explicit PreemptiveExecutor(
            const rclcpp::ExecutorOptions &options = rclcpp::ExecutorOptions(),
            size_t number_of_threads = 0,
            std::chrono::nanoseconds timeout = std::chrono::nanoseconds(-1));

        RCLCPP_PUBLIC
        virtual ~PreemptiveExecutor();

        /**
         * \sa rclcpp::Executor:spin() for more details
         * \throws std::runtime_error when spin() called while already spinning
         */
        RCLCPP_PUBLIC
        void
        spin() override;

        /// Get the number of threads in the thread pool
        RCLCPP_PUBLIC
        size_t
        get_number_of_threads() const;

    protected:
        /// Main execution loop for each thread
        /**
         * \param this_thread_number thread identifier for this execution thread
         */
        RCLCPP_PUBLIC
        void
        run(size_t this_thread_number);

        bool
        get_next_executable(rclcpp::AnyExecutable &any_executable, std::chrono::nanoseconds timeout = std::chrono::nanoseconds(-1));

    private:
        RCLCPP_DISABLE_COPY(PreemptiveExecutor)

        /// Get the next executable with priority and deadline consideration
        /**
         * \param any_exec reference to store the next executable
         * \param timeout maximum time to wait for work
         * \return true if executable was found, false otherwise
         */
        bool
        get_next_ready_executable(
            rclcpp::AnyExecutable &any_executable);

        void
        wait_for_work(std::chrono::nanoseconds timeout);

        /// Thread synchronization mutex
        std::mutex wait_mutex_;

        /// Number of threads in the thread pool
        size_t number_of_threads_;

        /// Timeout for getting next executable
        std::chrono::nanoseconds next_exec_timeout_;

        /// TODO : Need to add priority and deadline maps based on README data
    };

} // namespace preemptive_executor

#endif // PREEMPTIVE_EDF_EXECUTOR__PREEMPTIVE_EDF_EXECUTOR_HPP_
