//
// Created by sleepwalker on 2023/12/30.
//

#ifndef DEMO_THREAD_QUEUE_FACTORY_H
#define DEMO_THREAD_QUEUE_FACTORY_H

#include "thread_queue_safe.h"
#include "function_wrapper.h"
namespace thread_pool {
    namespace thread_queue {
        class thread_queue_factory {
        public:
            static std::unique_ptr<thread_queue_base<function_wrapper>> create_safe_queue()
            {
                return std::make_unique<thread_queue_safe<function_wrapper>>();
            }
        };
    }
}
#endif //DEMO_THREAD_QUEUE_FACTORY_H