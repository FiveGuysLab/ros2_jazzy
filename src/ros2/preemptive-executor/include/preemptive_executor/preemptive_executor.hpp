#ifndef PREEMPTIVE_EXECUTOR
#define PREEMPTIVE_EXECUTOR

#include <rmw/rmw.h>

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
#include "rclcpp/visibility_control.hpp"

namespace preemptive_executor
{
    class WorkerGroup {
        public:
            //constructor for WorkerGroup should take in a vector of thread ids and instantiate the semaphore to make those thread id wait on it
            WorkerGroup(std::vector<pid_t> thread_ids): thread_ids(thread_ids), semaphore(std::make_shared<std::counting_semaphore>(thread_ids.size())) {}
            ~WorkerGroup();
            std::vector<pid_t> thread_ids;
            std::shared_ptr<std::counting_semaphore> semaphore;
    };

    struct ReadyQueue {
        std::mutex mutex;
        std::queue<rclcpp::AnyExecutable> queue;
    };

    struct ThreadAttributes {
        public:
            ThreadAttributes(int thread_id, int deadline, int period, int runtime): thread_id(thread_id), deadline(deadline), period(period), runtime(runtime) {}
            int deadline;
            int period;
            int runtime;           
    };


    class PreemptiveExecutor : public rclcpp::Executor
    {
    public:
        RCLCPP_SMART_PTR_DEFINITIONS(PreemptiveExecutor)

        //constructor for PreemptiveExecutor
        RCLCPP_PUBLIC explicit PreemptiveExecutor(); //leaving ctor params blank for now

        RCLCPP_PUBLIC virtual ~PreemptiveExecutor();
        RCLCPP_PUBLIC size_t get_number_of_threads() const;

        // // Timeout for getting next executable
        // std::chrono::nanoseconds next_exec_timeout_;

    protected:
        RCLCPP_PUBLIC void spin() override;
        RCLCPP_PUBLIC void run(size_t this_thread_number);
        bool get_next_executable(rclcpp::AnyExecutable &any_executable, std::chrono::nanoseconds timeout = std::chrono::nanoseconds(-1));
        bool get_next_ready_executable( rclcpp::AnyExecutable &any_executable);
        void wait_for_work(std::chrono::nanoseconds timeout);

        //helper methods for preemptive executor

    private:
        RCLCPP_DISABLE_COPY(PreemptiveExecutor)
        
        //data structures for preemptive executor
        std::unordered_map<int, *ThreadGroup> chain_thead_group_map; 
        std::unordered_map<int, *ThreadGroup> callback_id_thead_group_map; 
        std::unordered_map<int, int> callback_id_chain_map; 
        std::unordered_map<*ThreadGroup, vector<*WorkerGroup>> thread_group_worker_map; 
        std::unordered_map<CallbackGroup, std::mutex> callback_group_mutex_map; //for mutually exclusive cg's we need a mutex
        std::unordered_map<int, vector<WorkerGroup>> chain_id_worker_map; 
        std::unordered_map<int, ReadyQueue> chain_id_ready_queue_map; 
        std::unordered_map<*ThreadGroup, ThreadAttributes> thread_group_attributes_map; 

        //TODO: need a map bw chain id and ready set after ready set is defined 
    };

} 

#endif 
