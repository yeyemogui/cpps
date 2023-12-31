#ifndef DEMO_THREAD_QUEUE_SAFE_H
#define DEMO_THREAD_QUEUE_SAFE_H

#include <mutex>
#include <condition_variable>
#include "thread_queue_base.h"
namespace thread_pool {
    namespace thread_queue {
        template<typename T>
        class thread_queue_safe: public thread_queue_base<T>{
        private:
            struct node {
                std::unique_ptr <T> data;
                std::unique_ptr <node> next;
            };

            std::unique_ptr <node> head;
            node *tail;
            std::atomic<unsigned int> m_size;

            std::mutex head_mutex;
            std::mutex tail_mutex;
            std::condition_variable data_cond;

            auto get_tail() {
                std::lock_guard <std::mutex> tail_lock(tail_mutex);
                return tail;
            }

            auto pop_head() -> decltype(head)
            {
                auto old_head = std::move(head);
                head = std::move(old_head->next);
                m_size--;
                return old_head;
            }

            auto wait_for_data() {
                std::unique_lock <std::mutex> head_lock(head_mutex);
                data_cond.wait(head_lock, [&] { return head.get() != get_tail(); });
                return head_lock;
            }

            auto wait_pop_head() {
                std::unique_lock <std::mutex> head_lock(wait_for_data());
                return pop_head();
            }

        public:
            thread_queue_safe() : head(new node), tail(head.get()), m_size(0), thread_queue_base<T>() {}

            thread_queue_safe(const thread_queue_safe &other) = delete;

            thread_queue_safe &operator=(const thread_queue_safe &other) = delete;

            std::unique_ptr<T> wait_and_pop() override
            {
                auto old_head = wait_pop_head();
                return std::move(old_head->data);
            }

            void push(T &&new_value) override
            {
                auto data = std::make_unique<T>(std::move(new_value));
                std::unique_ptr <node> p(new node);
                node *const new_tail = p.get();
                {
                    std::lock_guard <std::mutex> tail_lock(tail_mutex);
                    tail->data = std::move(data);
                    tail->next = std::move(p);
                    tail = new_tail;
                    ++m_size;
                }
                data_cond.notify_one();
            }

            void clear() override
            {
                std::lock(head_mutex, tail_mutex);
                std::lock_guard <std::mutex> head_lock(head_mutex, std::adopt_lock);
                std::lock_guard <std::mutex> tail_lock(tail_mutex, std::adopt_lock);
                while (head.get() != tail) {
                    pop_head();
                }
            }

            unsigned int size() override
            {
                return m_size;
            }
            virtual ~thread_queue_safe() {
                clear();
            }
        };
    }
}
#endif //DEMO_THREAD_QUEUE_SAFE_H