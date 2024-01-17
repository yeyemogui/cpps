//
// Created by sleepwalker on 2024/1/12.
//

#ifndef DEMO_THREADSTACKLOCKFREEV1_BUGVERSION_H
#define DEMO_THREADSTACKLOCKFREEV1_BUGVERSION_H

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
        class ThreadStackLockFreeV1_BugVersion : public DataContainerBase<T>
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
            ThreadStackLockFreeV1_BugVersion() : head_(nullptr), size_(0), to_be_deleted_(nullptr) {}

            ~ThreadStackLockFreeV1_BugVersion() { clear(); }

            void push(T &&data) override {
                auto *node = new Node();
                node->data = std::make_unique<T>(std::move(data));
                node->next = head_.load(std::memory_order_relaxed);
                while (!head_.compare_exchange_weak(node->next, node, std::memory_order_acq_rel));
                size_.fetch_add(1, std::memory_order_relaxed);
            }

            void clear() override {
                while (try_pop());
                tryCleanToBeDeletedList();
            }

            unsigned int size() override {
                auto size = size_.load(std::memory_order_relaxed);
                return size;
            }

            std::unique_ptr<T> wait_and_pop() override {
                throw UnsupportedOperation();
                return nullptr;
            }

            std::unique_ptr<DataContainerBase<T>> clone() override {
                return std::make_unique<ThreadStackLockFreeV1_BugVersion<T>>();
            }

            std::unique_ptr<T> try_pop() override {
                Node *old_head = head_.load(std::memory_order_acquire);
                auto *hazard_record = getHazardPtrForThread();
                Node *temp = nullptr;
                do
                {
                    if (!old_head)
                    {
                        return nullptr;
                    }
                    temp = old_head;
                    hazard_record->store(temp, std::memory_order_release);
                    old_head = head_.load(std::memory_order_acquire);
                } while (temp != old_head);

                while (old_head && !head_.compare_exchange_strong(old_head, old_head->next, std::memory_order_seq_cst));
                size_.fetch_sub(1, std::memory_order_relaxed);
                hazard_record->store(nullptr, std::memory_order_release);
                std::unique_ptr<T> res;
                if (old_head)
                {
                    res.swap(old_head->data);
                }

                if (isDeletable(old_head))
                {
                    delete old_head;
                }
                else
                {
                    addToBeDeletedList(old_head);
                }
                tryCleanToBeDeletedList();
                return res;
            }
        };
    } // namespace lock_free
} // namespace thread_pool
#endif // DEMO_THREADSTACKLOCKFREEV1_BUGVERSION_H
