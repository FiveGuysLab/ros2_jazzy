#ifndef PREEMPTIVE_EDF_EXECUTOR__PREEMPTIVE_EDF_EXECUTOR_HPP_
#define PREEMPTIVE_EDF_EXECUTOR__PREEMPTIVE_EDF_EXECUTOR_HPP_

#include <rclcpp/rclcpp.hpp>
#include <memory>
#include <vector>
#include <chrono>

namespace preemptive_edf_executor
{

    /**
     * @brief Preemptive EDF (Earliest Deadline First) Executor
     *
     * This executor implements a preemptive EDF scheduling algorithm
     * for ROS2 nodes, prioritizing tasks based on their deadlines.
     */
    class PreemptiveEDFExecutor : public rclcpp::Executor
    {
    public:
        /**
         * @brief Constructor
         * @param options Executor options
         */
        explicit PreemptiveEDFExecutor(const rclcpp::ExecutorOptions &options = rclcpp::ExecutorOptions());

        /**
         * @brief Destructor
         */
        ~PreemptiveEDFExecutor() override = default;

        /**
         * @brief Execute any available work
         * @param max_duration Maximum time to spend executing
         * @return True if work was executed, false otherwise
         */
        bool execute_any_work(std::chrono::nanoseconds max_duration = std::chrono::nanoseconds(0)) override;

    protected:
        /**
         * @brief Get the next executable to run based on EDF scheduling
         * @return The next executable to run, or nullptr if none available
         */
        rclcpp::AnyExecutable get_next_executable();

        /**
         * @brief Calculate the deadline for a given executable
         * @param executable The executable to calculate deadline for
         * @return The deadline as a time point
         */
        std::chrono::steady_clock::time_point calculate_deadline(
            const rclcpp::AnyExecutable &executable);

    private:
        // EDF-specific scheduling logic would go here
        std::vector<rclcpp::AnyExecutable> executable_queue_;
    };

} // namespace preemptive_edf_executor

#endif // PREEMPTIVE_EDF_EXECUTOR__PREEMPTIVE_EDF_EXECUTOR_HPP_
