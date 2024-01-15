//
// Created by sleepwalker on 2024/1/14.
//

#ifndef DEMO_THREADSTACKLOCKFREEV2_H
#define DEMO_THREADSTACKLOCKFREEV2_H
#include "DataContainerBase.h"
#include "exceptions.h"

namespace thread_pool
{
    namespace lock_free
    {
        template<typename T>
        class ThreadStackLockFreeV2 : public DataContainerBase<T>
        {
        private:
            struct Node
            {
                std::unique_ptr<T> data;
                std::shared_ptr<Node> next;
            };
            std::shared_ptr<Node> head_;
            std::atomic<unsigned int> size_;
        public:
            ThreadStackLockFreeV2():head_(nullptr), size_(0)
            {
                auto test = std::make_shared<Node>();
                if(std::atomic_is_lock_free(&test))
                {
                    //std::cout << "Awesome, the system support atomic operation on shared_ptr!" << std::endl;
                }
                else
                {
                    //std::cout << "Sadly, the system does not support atomic operation on shared_ptr:(" << std::endl;
                }
            }
            void push(T&& data) override
            {
                auto old_head = std::atomic_load(&head_);
                auto node = std::make_shared<Node>();
                node->data = std::make_unique<T>(std::move(data));
                node->next = old_head;
                while(!std::atomic_compare_exchange_weak(&head_, &node->next, node));
                size_++;
            }

            std::unique_ptr<T> wait_and_pop() override
            {
                throw UnsupportedOperation();
                return nullptr;
            }

            std::unique_ptr<T> try_pop() override
            {
                auto old_head = std::atomic_load(&head_);
                while(old_head && !std::atomic_compare_exchange_weak(&head_, &old_head, old_head->next));
                if(!old_head)
                {
                    return nullptr;
                }
                size_--;
                old_head->next = nullptr;
                return std::move(old_head->data);
            }

            std::unique_ptr<DataContainerBase<T>> clone() override
            {
                return std::make_unique<ThreadStackLockFreeV2<T>>();
            }

            void clear() override
            {
                while(try_pop());
            }

            unsigned int size() override{
                return size_.load(std::memory_order_acquire);
            }
        };
    }
}
#endif //DEMO_THREADSTACKLOCKFREEV2_H
