//
// Created by sleepwalker on 2023/12/30.
//

#ifndef DEMO_DATACONTAINERFACTORY_H
#define DEMO_DATACONTAINERFACTORY_H

#include "ThreadQueueLocked.h"
#include "function_wrapper.h"
#include "ThreadStackLocked.h"
#include "ThreadStackLockFree.h"
#include "ThreadQueueLockFree.h"

namespace thread_pool
{
    class DataContainerFactory
    {
    public:
        static std::unique_ptr<DataContainerBase<function_wrapper>> create_safe_queue() {
            return std::make_unique<thread_queue::ThreadQueueLocked<function_wrapper>>();
        }

        static std::unique_ptr<DataContainerBase<function_wrapper>> create_stack_locked() {
            return std::make_unique<thread_stack::ThreadStackLocked<function_wrapper>>();
        }

        static std::unique_ptr<DataContainerBase<function_wrapper>> create_stack_lock_free() {
            return std::make_unique<lock_free::ThreadStackLockFree<function_wrapper>>();
        }

        static std::unique_ptr<DataContainerBase<function_wrapper>> create_queue_lock_free() {
            return std::make_unique<lock_free::ThreadQueueLockFree<function_wrapper>>();
        }
    };
}
#endif //DEMO_DATACONTAINERFACTORY_H