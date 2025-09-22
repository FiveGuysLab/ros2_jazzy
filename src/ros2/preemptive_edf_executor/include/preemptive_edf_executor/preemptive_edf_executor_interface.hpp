#ifndef PREEMPTIVE_EDF_EXECUTOR_INTERFACE
#define PREEMPTIVE_EDF_EXECUTOR_INTERFACE

enum ExecutableType
{
  SUBSCRIPTION,
  SERVICE,
  CLIENT,
  TIMER,
  WAITABLE
};

class PreemptiveEDFExecutable {
public:
    std::shared_ptr<const void> handle;
    ExecutableType type;
    int deadline;
    int period;
    int runtime;
    int chain_id;

    std::shared_ptr<rclcpp::TimerBase> timer_handle;

    PreemptiveEDFExecutable(std::shared_ptr<const void> handle, ExecutableType type, int deadline, int period, int runtime, int chain_id) : handle(handle), type(type), deadline(deadline), period(period), runtime(runtime), chain_id(chain_id) {}

    // compare operator overload 
    bool operator==(const PreemptiveEDFExecutable &other) const;

};

template <typename Alloc = std::allocator<void>> //template to allow for custom memory allocator
class PreemptiveEDFExecutorInterface : rclcpp::memory_strategy::MemoryStrategy
{
public:
    //constructor
    explicit PreemptiveEDFExecutorInterface(std::shared_ptr<Alloc> allocator) : allocator_(allocator) {}
    PreemptiveEDFExecutorInterface() : allocator_(make_shared<Alloc>()) {}

    void set_callback_details(std::shared_ptr<const void> callback, ExecutableType type, int deadline, int period, int runtime, int chain_id); 


private:
    std::shared_ptr<VoidAlloc> allocator_;

    //maps for data from algorithm
    std::map<std::shared_ptr<const void>, std::vector<int>> callback_to_chain_ids_;
    std::map<int, std::vector<PreemptiveEDFExecutable>> chain_to_executables_;
    std::map<int, std::vector<int>> chain_to_threads_;

    // storage => required for ROS2 compatibility
    std::vector<std::shared_ptr<const rcl_subscription_t>> subscription_handles_;
    std::vector<std::shared_ptr<const rcl_service_t>> service_handles_;
    std::vector<std::shared_ptr<const rcl_client_t>> client_handles_;
    std::vector<std::shared_ptr<const rcl_timer_t>> timer_handles_;
    std::vector<std::shared_ptr<rclcpp::Waitable>> waitable_handles_;
    std::vector<const rclcpp::GuardCondition *> guard_conditions_;

    // helper methods => SUBJECT TO CHANGE
    void store_handle(std::shared_ptr<const void> handle, ExecutableType type);     
    std::shared_ptr<PreemptiveEDFExecutable> get_and_reset_executable(std::shared_ptr<const void> executable, ExecutableType t);
    void add_executable_to_ready_queue(std::shared_ptr<PreemptiveEDFExecutable> executable);
};


#endif
