//
// Created by sleepwalker on 2024/1/13.
//

#ifndef DEMO_THREADQUEUELOCKFREE_H
#define DEMO_THREADQUEUELOCKFREE_H

#include "DataContainerBase.h"
#include "exceptions.h"
#include <map>
#include <thread>

namespace thread_pool
{
    namespace lock_free
    {

        template<typename T>
        class ThreadQueueLockFree : public DataContainerBase<T>
        {
        private:
            struct Node
            {
                std::unique_ptr<T> data;
                std::atomic<bool> isOperational;
                std::atomic<bool> isValid;
                Node *next;

                Node() {
                    data = nullptr;
                    next = nullptr;
                    isOperational = false;
                }
            };

            std::atomic<Node *> head_;
            std::atomic<Node *> tail_;
            std::atomic<unsigned int> size_;
            std::map<std::thread::id, std::atomic<void *>> hazard_records_;
            std::mutex mutex_;

            std::atomic<Node *> to_be_deleted;

            std::atomic<void *> *getHazardPtr() {
                static thread_local std::atomic<void *> *hazard_ptr_ = [this] {
                    std::lock_guard<std::mutex> g(this->mutex_);
                    auto threadId = std::this_thread::get_id();
                    this->hazard_records_.emplace(threadId, nullptr);
                    return &this->hazard_records_[threadId];
                }();
                return hazard_ptr_;
            }

            bool isDeletable(Node *node) {
                return hazard_records_.cend() ==
                       std::find_if(hazard_records_.cbegin(),
                                    hazard_records_.cend(),
                                    [node](auto &r) {
                                        return r.second.load(
                                                std::memory_order_acquire) == node;
                                    })
                       ? true
                       : false;
            }

            void addToBeDeleted(Node *node) {
                auto *old = to_be_deleted.load(std::memory_order_acquire);
                node->next = old;
                while (!to_be_deleted.compare_exchange_weak(node->next, node,
                                                            std::memory_order_acq_rel));
            }

            void tryCleanToBeDeletedList() {
                auto *node = to_be_deleted.exchange(nullptr);
                while (node)
                {
                    auto *next = node->next;
                    if (isDeletable(node))
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
            ThreadQueueLockFree() {
                Node *node = new Node();
                head_.store(node);
                tail_.store(node);
            }

            ~ThreadQueueLockFree() { clear(); }

            void push(T &&data) override {
                Node *new_tail = new Node();
                auto d = std::make_unique<T>(std::move(data));
                while (true)
                {
                    auto *old_tail = tail_.load(std::memory_order_acquire);
                    bool test = false;
                    if (!old_tail->isOperational.compare_exchange_strong(
                            test, true, std::memory_order_acq_rel))
                    {
                        continue;
                    }
                    old_tail->next = new_tail;
                    old_tail->data = std::move(d);
                    old_tail->isValid.store(true, std::memory_order_release);
                    tail_.store(new_tail, std::memory_order_release);
                    size_++;
                    break;
                }
            }

            std::unique_ptr<T> try_pop() override {
                Node *old_head = head_.load(std::memory_order_acquire);
                auto *hazardPtr = getHazardPtr();
                Node* temp = nullptr;
                do
                {
                    temp = old_head;
                    hazardPtr->store(temp, std::memory_order_release);
                    old_head = head_.load(std::memory_order_acquire);
                }while(temp != old_head);

                while (old_head->isValid.load(std::memory_order_acquire) && !head_.compare_exchange_weak(old_head, old_head->next, std::memory_order_acq_rel));

                if(old_head == head_.load(std::memory_order_acquire))
                {
                    hazardPtr->store(nullptr, std::memory_order_release);
                    return nullptr;
                }
                hazardPtr->store(nullptr, std::memory_order_release);
                size_--;
                std::unique_ptr<T> res = std::move(old_head->data);

                if (isDeletable(old_head))
                {
                    delete old_head;
                }
                else
                {
                    addToBeDeleted(old_head);
                }
                tryCleanToBeDeletedList();
                return res;
            }

            void clear() override {
                std::unique_ptr<function_wrapper> data;
                do
                {
                    data = try_pop();
                } while (data);
            }

            unsigned int size() override { return size_.load(std::memory_order_acquire); }

            std::unique_ptr<T> wait_and_pop() override {
                throw UnsupportedOperation();
                return nullptr;
            }

            std::unique_ptr<DataContainerBase<T>> clone() override {
                return std::make_unique<ThreadQueueLockFree<T>>();
            }
        };
    } // namespace lock_free
} // namespace thread_pool
#endif // DEMO_THREADQUEUELOCKFREE_H
