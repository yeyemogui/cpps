//
// Created by sleepwalker on 2024/1/15.
//

#ifndef DEMO_THREADSTACKLOCKFREEV3_H
#define DEMO_THREADSTACKLOCKFREEV3_H
#include "DataContainerBase.h"
#include "exceptions.h"
namespace thread_pool
{
    namespace lock_free
    {
        template<typename T>
        class ThreadStackLockFreeV3 : public DataContainerBase<T>
        {
        private:
            struct RefNode;
            struct Node
            {
                std::unique_ptr<T> data;
                std::atomic<int> internalRefNum;
                RefNode next;
                Node()
                {
                    internalRefNum = 0;
                    data = nullptr;
                }
            };

            struct RefNode
            {
                Node* core;
                unsigned int refNum;
                RefNode()
                {
                    core = nullptr;
                    refNum = 0;
                }
            };
            std::atomic<RefNode> head_;
            std::atomic<unsigned int> size_;
        public:
            ThreadStackLockFreeV3(): head_(RefNode()), size_(0){}
            std::unique_ptr<T> wait_and_pop() override
            {
                throw UnsupportedOperation();
                return nullptr;
            }

            std::unique_ptr<DataContainerBase<T>> clone() override
            {
                return std::make_unique<ThreadStackLockFreeV3<T>>();
            }

            unsigned int size() override
            {
                return size_.load(std::memory_order_acquire);
            }

            void clear() override
            {
                while (try_pop());
            }

            ~ThreadStackLockFreeV3()
            {
                clear();
            }

            void push(T&& data) override
            {
                RefNode refNode;
                refNode.core = new Node();
                refNode.core->data = std::make_unique<T>(std::move(data));
                refNode.core->next = head_.load(std::memory_order_acquire);
                refNode.refNum = 0;
                while(!head_.compare_exchange_weak(refNode.core->next, refNode, std::memory_order_acq_rel));
                size_++;
            }

            std::unique_ptr<T> try_pop() override
            {
                auto old_head = head_.load(std::memory_order_acquire);
                RefNode updated_head;
                do
                {
                    if(!old_head.core)
                    {
                        return nullptr;
                    }
                    updated_head = old_head;
                    ++updated_head.refNum;
                }while(!head_.compare_exchange_weak(old_head, updated_head));

                if(head_.compare_exchange_strong(updated_head, updated_head.core->next))
                {
                    size_--;
                    std::unique_ptr<T> res = std::move(updated_head.core->data);
                    auto refNum = updated_head.refNum - 1;
                    updated_head.core->internalRefNum.fetch_add(refNum, std::memory_order_acq_rel);
                    if(updated_head.core->internalRefNum.load(std::memory_order_acquire) == 0)
                    {
                        delete updated_head.core;
                    }
                    return res;
                }
                else
                {
                    if(old_head.core->internalRefNum.fetch_sub(1, std::memory_order_acquire) == 1)
                    {
                        delete old_head.core;
                    }
                }
                return nullptr;
            }
        };
    }
}
#endif //DEMO_THREADSTACKLOCKFREEV3_H
