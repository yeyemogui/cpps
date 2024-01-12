//
// Created by sleepwalker on 2024/1/13.
//

#ifndef DEMO_THREADQUEUELOCKFREE_H
#define DEMO_THREADQUEUELOCKFREE_H
#include "DataContainerBase.h"
#include <thread>
#include "exceptions.h"
#include <map>
namespace thread_pool
{
    namespace lock_free
    {

        template<typename T>
        class ThreadQueueLockFree: public DataContainerBase<T>
        {
        private:
            struct Node
            {
                std::unique_ptr<T> data;
                Node* prev;
                Node* next;
            };
            std::atomic<Node*> head_;
            std::atomic<Node*> tail_;
            std::atomic<unsigned int> size_;
            std::map<std::thread::id, std::atomic<void*>> hazard_records_;
            std::mutex mutex_;

            std::atomic<Node*> to_be_deleted;
            std::atomic<void*>* getHazardPtr()
            {
                static thread_local std::atomic<void*>* hazard_ptr_  = [this]{
                    std::lock_guard<std::mutex> g(this->mutex_);
                    auto threadId = std::this_thread::get_id();
                    this->hazard_records_.emplace(threadId, nullptr);
                    return &this->hazard_records_[threadId];}();
                return hazard_ptr_;
            }

            bool isDeletable(Node* node)
            {
                return hazard_records_.cend() == std::find_if(hazard_records_.cbegin(), hazard_records_.cend(), [node](auto& r){
                    return r.second.load(std::memory_order_acquire) == node;}) ? true: false;
            }

            void addToBeDeleted(Node* node)
            {
                auto* old = to_be_deleted.load(std::memory_order_acquire);
                while(!to_be_deleted.compare_exchange_weak(old, node, std::memory_order_acq_rel));
            }

            void tryCleanToBeDeletedList()
            {
                auto* node = to_be_deleted.exchange(nullptr);
                while(node)
                {
                    auto* next = node->next;
                    if(isDeletable(node))
                    {
                        delete node;
                    }
                    else
                    {
                        addToBeDeleted(node);
                    }
                    node = next;
                }
            }

        public:
            void push(T&& data) override
            {
                auto* node = new Node();
                node->data = std::make_unique<T>(std::move(data));
                auto old_tail = tail_.load(std::memory_order_acquire);
                node->prev = old_tail;
                while(!tail_.compare_exchange_weak(node->prev, node, std::memory_order_acq_rel));
                size_++;
            }

            std::unique_ptr<T> try_pop() override
            {
                auto* old = head_.load(std::memory_order_acquire);
                auto* hazardPtr = getHazardPtr();
                hazardPtr->store(old, std::memory_order_release);
                while(old && !head_.compare_exchange_weak(old, old->next, std::memory_order_acq_rel));
                hazardPtr->store(nullptr, std::memory_order_release);
                if(old)
                {
                    size_--;
                    std::unique_ptr<T> res;
                    res.swap(old->data);
                    if(isDeletable(old))
                    {
                        delete old;
                    }
                    else
                    {
                        addToBeDeleted(old);
                    }
                    tryCleanToBeDeletedList();
                    return res;
                }
                return nullptr;
            }

            void clear() override
            {
                std::unique_ptr<function_wrapper> data;
                do {
                    data = try_pop();
                }while(data);
            }

            unsigned int size() override
            {
                return size_.load(std::memory_order_acquire);
            }

            std::unique_ptr<T> wait_and_pop() override
            {
                throw UnsupportedOperation();
                return nullptr;
            }

            std::unique_ptr<DataContainerBase<T>> clone() override
            {
                return std::make_unique<ThreadQueueLockFree<T>>();
            }
        };
    }
}
#endif //DEMO_THREADQUEUELOCKFREE_H
