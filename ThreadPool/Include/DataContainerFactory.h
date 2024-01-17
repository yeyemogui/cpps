//
// Created by sleepwalker on 2023/12/30.
//

#ifndef DEMO_DATACONTAINERFACTORY_H
#define DEMO_DATACONTAINERFACTORY_H

#include "ThreadQueueLocked.h"
#include "function_wrapper.h"
#include "ThreadStackLocked.h"
#include "ThreadStackLockFreeV1_BugVersion.h"
#include "ThreadStackLockFreeV2.h"
#include "ThreadStackLockFreeV3.h"
#include "ThreadQueueLockFree_BugVersion.h"
#include "ThreadQueueLockFreeV2.h"

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

        static std::unique_ptr<DataContainerBase<function_wrapper>> create_stack_lock_freeV1() {
            return std::make_unique<lock_free::ThreadStackLockFreeV1_BugVersion<function_wrapper>>();
        }

        static std::unique_ptr<DataContainerBase<function_wrapper>> create_stack_lock_freeV2() {
            return std::make_unique<lock_free::ThreadStackLockFreeV2<function_wrapper>>();
        }

        static std::unique_ptr<DataContainerBase<function_wrapper>> create_stack_lock_freeV3() {
            return std::make_unique<lock_free::ThreadStackLockFreeV3<function_wrapper>>();
        }

        static std::unique_ptr<DataContainerBase<function_wrapper>> create_queue_lock_free() {
            return std::make_unique<lock_free::ThreadQueueLockFree_BugVersion<function_wrapper>>();
        }

        static std::unique_ptr<DataContainerBase<function_wrapper>> create_queue_lock_freeV2() {
            return std::make_unique<lock_free::ThreadQueueLockFreeV2<function_wrapper>>();
        }
    };
}
#endif //DEMO_DATACONTAINERFACTORY_H