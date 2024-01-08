//
// Created by sleepwalker on 2024/1/6.
//

#ifndef DEMO_THREADSTACKLOCKED_H
#define DEMO_THREADSTACKLOCKED_H
#include "DataContainerBase.h"
#include <thread>
namespace thread_pool
{
    namespace thread_stack
    {
        template<typename T>
        class ThreadStackLocked: public DataContainerBase<T>
        {
        private:
            struct Node
            {
                std::unique_ptr<Node> next;
                std::unique_ptr<T> data;
            };
            std::unique_ptr<Node> head_;
            std::mutex mutex_;
            std::condition_variable data_cond;
            std::atomic<unsigned int> size_;
            auto pop_head() -> decltype(head_)
            {
                std::unique_ptr<Node> temp = nullptr;
                if(head_ != nullptr)
                {
                    temp = std::move(head_);
                    head_ = std::move(temp->next);
                    size_--;
                }
                return temp;
            }

            bool is_empty()
            {
                return head_ == nullptr;
            }
        public:
            ThreadStackLocked(): head_(nullptr){};
            void push(T&& data) override
            {
                auto temp = std::make_unique<Node>();
                temp->data = std::make_unique<T>(std::move(data));
                {
                    std::lock_guard<std::mutex> guard(mutex_);
                    temp->next = std::move(head_);
                    head_ = std::move(temp);
                    size_++;
                }
                data_cond.notify_one();
            }

            std::unique_ptr<T> wait_and_pop() override
            {
                std::unique_ptr<Node> temp;
                {
                    std::unique_lock<std::mutex> locker(mutex_);
                    data_cond.wait(locker, [&] { return !is_empty(); });
                    temp = pop_head();
                }
                return std::move(temp->data);
            }

            std::unique_ptr<T> try_pop() override
            {
                std::unique_ptr<Node> temp;
                {
                    std::unique_lock<std::mutex> locker(mutex_);
                    temp = pop_head();
                }
                return temp == nullptr? nullptr: std::move(temp->data);
            }

            void clear() override
            {
                std::unique_lock<std::mutex> locker(mutex_);
                while(!is_empty())
                {
                    auto temp = pop_head();
                }
            }

            std::unique_ptr<DataContainerBase<T>> clone() override
            {
                return std::make_unique<ThreadStackLocked<T>>();
            }

            unsigned int size() override
            {
                return size_;
            }

            virtual ~ThreadStackLocked()
            {
                clear();
            }
        };
    }
}
#endif //DEMO_THREADSTACKLOCKED_H
