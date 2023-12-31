//
// Created by sleepwalker on 2023/12/30.
//

#ifndef DEMO_THREAD_QUEUE_BASE_H
#define DEMO_THREAD_QUEUE_BASE_H

#include <memory>
namespace thread_pool{
    namespace thread_queue{
        template<typename T>
        class thread_queue_base
        {
        public:
            virtual void push(T&&) = 0;
            virtual void clear() = 0;
            virtual unsigned int size() = 0;
            virtual std::unique_ptr<T> wait_and_pop() = 0;
            virtual ~thread_queue_base() = default;
        };
    }
}
#endif //DEMO_THREAD_QUEUE_BASE_H