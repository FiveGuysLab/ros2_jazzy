#ifndef PREEMPTIVE_EXECUTOR_INTERFACE
#define PREEMPTIVE_EXECUTOR_INTERFACE

enum ExecutableType
{
  SUBSCRIPTION,
  SERVICE,
  CLIENT,
  TIMER,
  WAITABLE
};

class PreemptiveExecutable {
public:
    std::shared_ptr<const void> handle;
    ExecutableType type;
    int deadline;
    int period;
    int runtime;
    int chain_id;

    std::shared_ptr<rclcpp::TimerBase> timer_handle;

    PreemptiveExecutable(std::shared_ptr<const void> handle, ExecutableType type, int deadline, int period, int runtime, int chain_id) : handle(handle), type(type), deadline(deadline), period(period), runtime(runtime), chain_id(chain_id) {}

    // compare operator overload 
    bool operator==(const PreemptiveExecutable &other) const;

};

template <typename Alloc = std::allocator<void>> //template to allow for custom memory allocator
class PreemptiveExecutorInterface : rclcpp::memory_strategy::MemoryStrategy
{
public:
    //constructor
    explicit PreemptiveExecutorInterface(std::shared_ptr<Alloc> allocator);
    PreemptiveExecutorInterface();

    //overrides from MemoryStrategy base class
    void add_guard_condition(const rclcpp::GuardCondition & guard_condition) override;
    void remove_guard_condition(const rclcpp::GuardCondition * guard_condition) override;
    void clear_handles() override;
    void remove_null_handles(rcl_wait_set_t *wait_set) override;
    bool collect_entities(const WeakNodeList &weak_nodes) override;
    void add_waitable_handle(const rclcpp::Waitable::SharedPtr &waitable) override;
    bool add_handles_to_wait_set(rcl_wait_set_t *wait_set) override;

    // commented some stuff out as it is undecided 

    /*
    // executable retrieval methods
    void get_next_executable(rclcpp::AnyExecutable &any_exec, const WeakNodeList &weak_nodes) override;
    void get_next_subscription(rclcpp::AnyExecutable &any_exec, const WeakNodeList &weak_nodes) override;
    void get_next_service(rclcpp::AnyExecutable &any_exec, const WeakNodeList &weak_nodes) override;
    void get_next_client(rclcpp::AnyExecutable &any_exec, const WeakNodeList &weak_nodes) override;
    void get_next_timer(rclcpp::AnyExecutable &any_exec, const WeakNodeList &weak_nodes) override;
    void get_next_waitable(rclcpp::AnyExecutable &any_exec, const WeakNodeList &weak_nodes) override;

    // EDF specific methods
    void post_execute(rclcpp::AnyExecutable any_exec, int thread_id = -1);
    void set_executable_deadline(std::shared_ptr<const void> handle, int deadline, int period, ExecutableType t, int chain_id);
    
    // chain management methods
    void add_callback_to_chain(std::shared_ptr<const void> callback, int chain_id);
    std::vector<int> get_chains_for_callback(std::shared_ptr<const void> callback);
    std::vector<PreemptiveEDFExecutable> get_executables_in_chain(int chain_id);
    */

private:
    std::shared_ptr<VoidAlloc> allocator_;

    //maps for data from algorithm
    std::map<std::shared_ptr<const void>, std::vector<int>> callback_to_chain_ids_;
    std::map<int, std::vector<PreemptiveExecutable>> chain_to_executables_;
    std::map<int, std::vector<int>> chain_to_threads_;

    // storage => required for ROS2 compatibility
    std::vector<std::shared_ptr<const rcl_subscription_t>> subscription_handles_;
    std::vector<std::shared_ptr<const rcl_service_t>> service_handles_;
    std::vector<std::shared_ptr<const rcl_client_t>> client_handles_;
    std::vector<std::shared_ptr<const rcl_timer_t>> timer_handles_;
    std::vector<std::shared_ptr<rclcpp::Waitable>> waitable_handles_;
    std::vector<const rclcpp::GuardCondition *> guard_conditions_;

    // helper methods => SUBJECT TO CHANGE
    std::shared_ptr<PreemptiveExecutable> get_and_reset_executable(std::shared_ptr<const void> executable, ExecutableType t);
    void add_executable_to_ready_queue(std::shared_ptr<PreemptiveExecutable> executable);
};


#endif
