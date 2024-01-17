//
// Created by sleepwalker on 2024/1/16.
//

#ifndef DEMO_THREADQUEUELOCKFREEV2_H
#define DEMO_THREADQUEUELOCKFREEV2_H
#include "DataContainerBase.h"
#include "exceptions.h"
namespace thread_pool
{
    namespace lock_free
    {
        template<typename T>
        class ThreadQueueLockFreeV2 : public DataContainerBase<T>
        {
        private:
            struct Node;
            struct RefNode
            {
                unsigned int refNum;
                Node* core;
                RefNode()
                {
                    refNum = 0;
                    core = nullptr;
                }
            };
            struct Node
            {
                std::atomic<T*> data;
                std::atomic<int> internalRefNum;
                RefNode next;
                Node()
                {
                    data = nullptr;
                    internalRefNum = 0;
                }
            };

            std::atomic<RefNode> head_;
            std::atomic<RefNode> tail_;
            std::atomic<unsigned int> size_;
        public:
            ThreadQueueLockFreeV2():size_(0)
            {
                RefNode refNode;
                refNode.core = new Node();
                head_ = refNode;
                tail_ = refNode;
            }

            ~ThreadQueueLockFreeV2()
            {
                clear();
            }
            void push(T&& data) override
            {
                RefNode newRefNode;
                newRefNode.core = new Node();
                auto* d = new T(std::move(data));
                auto old_tail = tail_.load(std::memory_order_acquire);
                while(true)
                {
                    T* test = nullptr;
                    if(old_tail.core->data.compare_exchange_strong(test, d))
                    {
                        old_tail.core->next = newRefNode;
                        tail_.store(newRefNode, std::memory_order_release);
                        size_++;
                        break;
                    }
                    else
                    {
                        old_tail = tail_.load(std::memory_order_release);
                    }
                }
            }

            std::unique_ptr<T> wait_and_pop() override
            {
                throw UnsupportedOperation();
                return nullptr;
            }

            unsigned int size() override
            {
                return size_.load(std::memory_order_relaxed);
            }

            void clear() override
            {
                while(try_pop());
            }

            std::unique_ptr<DataContainerBase<T>> clone() override
            {
                return std::make_unique<ThreadQueueLockFreeV2<T>>();
            }

            std::unique_ptr<T> try_pop() override
            {
                RefNode old_head = head_.load(std::memory_order_relaxed);
                RefNode temp_head;
                do
                {
                    temp_head = old_head;
                    if(tail_.compare_exchange_strong(temp_head, temp_head))
                    {
                        return nullptr;
                    }
                    temp_head = old_head;
                    ++temp_head.refNum;
                }while(!head_.compare_exchange_strong(old_head, temp_head, std::memory_order_seq_cst));
                if(temp_head.core && head_.compare_exchange_strong(temp_head, temp_head.core->next))
                {
                    size_--;
                    std::unique_ptr<T> res(old_head.core->data.load());
                    auto totalRefNum = temp_head.refNum - 1;
                    if(temp_head.core->internalRefNum.fetch_add(totalRefNum, std::memory_order_acq_rel) == -totalRefNum)
                    {
                        delete temp_head.core;
                    }
                    return res;
                }
                else
                {
                    if(temp_head.core->internalRefNum.fetch_sub(1, std::memory_order_acq_rel) == 1)
                    {
                        delete temp_head.core;
                    }
                }
                return nullptr;
            }
        };
    }
}
#endif //DEMO_THREADQUEUELOCKFREEV2_H
