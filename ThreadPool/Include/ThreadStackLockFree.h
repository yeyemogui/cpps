//
// Created by sleepwalker on 2024/1/12.
//

#ifndef DEMO_THREADSTACKLOCKFREE_H
#define DEMO_THREADSTACKLOCKFREE_H

#include "DataContainerBase.h"
#include "exceptions.h"
#include <list>
#include <map>
#include <thread>

namespace thread_pool
{
    namespace lock_free
    {
        template<typename T>
        class ThreadStackLockFree : public DataContainerBase<T>
        {
        private:
            struct Node
            {
                std::unique_ptr<T> data;
                Node *next;
            };
            std::atomic<Node *> head_;
            std::atomic<unsigned int> size_;
            std::map<std::thread::id, std::atomic<void *>> hazardRecords_;
            std::mutex mutex_;
            std::atomic<Node *> to_be_deleted_;

            std::atomic<void *> *getHazardPtrForThread() {
                thread_local static std::atomic<void *> *hazardPtr = [this] {
                    std::lock_guard<std::mutex> locker(mutex_);
                    auto threadId = std::this_thread::get_id();
                    hazardRecords_.emplace(threadId, nullptr);
                    return &hazardRecords_[threadId];
                }();
                return hazardPtr;
            }

            bool isDeletable(Node *ptr) {
                if (!ptr)
                {
                    return false;
                }
                auto ret =
                        hazardRecords_.cend() ==
                        std::find_if(hazardRecords_.cbegin(), hazardRecords_.cend(),
                                     [&](auto &r) {
                                         return r.second.load(
                                                 std::memory_order_acquire) == ptr;
                                     })
                        ? true
                        : false;
                return ret;
            }

            void addToBeDeletedList(Node *ptr) {
                if (!ptr)
                {
                    return;
                }
                ptr->next = to_be_deleted_.load(std::memory_order_relaxed);
                while (!to_be_deleted_.compare_exchange_weak(ptr->next, ptr, std::memory_order_acq_rel));
            }

            void tryCleanToBeDeletedList() {
                Node *old = to_be_deleted_.exchange(nullptr);
                while (old)
                {
                    auto *next = old->next;
                    if (isDeletable(old))
                    {
                        delete old;
                    }
                    else
                    {
                        addToBeDeletedList(old);
                    }
                    old = next;
                }
            }

        public:
            ThreadStackLockFree() = default;

            ~ThreadStackLockFree() { clear(); }

            void push(T &&data) override {
                auto *node = new Node();
                node->data = std::make_unique<T>(std::move(data));
                node->next = head_.load(std::memory_order_relaxed);
                while (!head_.compare_exchange_weak(node->next, node, std::memory_order_acq_rel));
                size_.fetch_add(1, std::memory_order_acquire);
            }

            void clear() override {
                while (auto data = try_pop());
            }

            unsigned int size() override {
                auto size = size_.load(std::memory_order_acquire);
                return size;
            }

            std::unique_ptr<T> wait_and_pop() override {
                throw UnsupportedOperation();
                return nullptr;
            }

            std::unique_ptr<DataContainerBase<T>> clone() override {
                return std::make_unique<ThreadStackLockFree<T>>();
            }

            std::unique_ptr<T> try_pop() override {
                Node *old = head_.load(std::memory_order_acquire);
                auto *hazard_record = getHazardPtrForThread();
                hazard_record->store(old);
                if (!old)
                {
                    return nullptr;
                }

                while (old && !head_.compare_exchange_strong(old, old->next,
                                                             std::memory_order_seq_cst))
                {
                }
                hazard_record->store(nullptr, std::memory_order_release);
                std::unique_ptr<T> res;
                if (old)
                {
                    res.swap(old->data);
                }

                if (isDeletable(old))
                {
                    delete old;
                }
                else
                {
                    addToBeDeletedList(old);
                }
                tryCleanToBeDeletedList();
                return res;
            }
        };
    } // namespace lock_free
} // namespace thread_pool
#endif // DEMO_THREADSTACKLOCKFREE_H
