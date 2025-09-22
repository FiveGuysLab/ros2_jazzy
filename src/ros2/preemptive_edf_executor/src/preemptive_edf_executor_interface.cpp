#include "preemptive_edf_executor/preemptive_edf_executor_interface.hpp"


bool PreemptiveEDFExecutable::operator==(const PreemptiveEDFExecutable &other) const
{
    return handle == other.handle;
}

template <>
void PreemptiveEDFExecutorInterface::set_callback_details(std::shared_ptr<const void> callback, ExecutableType type, int deadline, int period, int runtime, int chain_id)
{
    // create a new PreemptiveEDFExecutable
    PreemptiveEDFExecutable executable(callback, type, deadline, period, runtime, chain_id);
    callback_to_chain_ids_[callback] = chain_id;
    chain_to_executables_[chain_id].push_back(executable);
    chain_to_threads_[chain_id].push_back(std::this_thread::get_id());
    store_handle(callback, type);
}

// method for storing into respective handle vector based on type
void PreemptiveEDFExecutorInterface::store_handle(std::shared_ptr<const void> handle, ExecutableType type)
{
    switch (type)
    {
        case ExecutableType::SUBSCRIPTION:
            subscription_handles_.push_back(handle);
            break;
        case ExecutableType::SERVICE:
            service_handles_.push_back(handle);
            break;
        case ExecutableType::CLIENT:
            client_handles_.push_back(handle);
            break;
        case ExecutableType::TIMER:
            timer_handles_.push_back(handle);
            break;
        case ExecutableType::WAITABLE:
            waitable_handles_.push_back(handle);
            break;
    }
}