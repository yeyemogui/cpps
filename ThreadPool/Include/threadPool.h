//
// Created by sleepwalker on 2023/12/27.
//

#ifndef DEMO_THREADPOOL_H
#define DEMO_THREADPOOL_H
#include "thread_queue_safe.h"
#include "function_wrapper.h"
#include <future>
#include <algorithm>
#include <execution>
#include "exceptions.h"
namespace thread_pool {
    class threadPool {
    private:
        std::unique_ptr<thread_queue::thread_queue_base<function_wrapper>> work_queue_;
        std::atomic<bool> done_ = false;
        std::vector <std::thread> threads_;
        unsigned int threadNum_;

        void worker() {
            while (!done_) {
                auto task = work_queue_->wait_and_pop();
                task->run();
            }
        }

        template<typename R, typename F>
        auto submitWrapper(F f) {
            std::packaged_task < R() > task(std::move(f));
            std::future <R> res(task.get_future());
            work_queue_->push(function_wrapper(std::move(task)));
            return res;
        }

    public:
        explicit threadPool(std::unique_ptr<thread_queue::thread_queue_base<function_wrapper>> queue, unsigned int threadNum = 0) : work_queue_(std::move(queue)), done_(false) {
            threadNum_ = std::min(threadNum, std::thread::hardware_concurrency());
            try {
                for (unsigned int i = 0; i < threadNum_; i++) {
                    threads_.emplace_back(&threadPool::worker, this);
                }
            }
            catch (...) {
                stop();
                throw;
            }
        }

        ~threadPool() {
            stop();
            /* this is not good, because sometimes exception get thrown, need to judge joinable before join
#ifdef DARWIN
            std::for_each(threads_.begin(), threads_.end(), std::mem_fn(&std::thread::join));
#else
            std::for_each(std::execution::par, threads_.begin(), threads_.end(), std::mem_fn(&std::thread::join));
#endif
             */
            for(auto& thread : threads_)
            {
                if(thread.joinable())
                {
                    thread.join();
                }
            }
            std::cout << "All threads in pool are stopped" << std::endl;
        }

        template<typename R, typename T, typename... Types>
        auto submit(R (T::*f)(Types...), T object, Types... args) {
            auto func = std::bind(f, object, args...);
            return submitWrapper<R>(func);
        }

        template<typename R, typename T>
        auto submit(R (T::*f)(), T object) {
            return submitWrapper<R>([&object, f] { return (object.*f)(); });
        }

        template<typename T>
        struct isSmartPtr : public std::false_type{};
        template<typename T>
        struct isSmartPtr<std::unique_ptr<T>>: public std::true_type {};
        template<typename T>
        struct isSmartPtr<std::shared_ptr<T>>: public std::true_type {};

        template<typename R, typename T, typename = std::enable_if_t<isSmartPtr<T>::value>>
        auto submit(T smartPtr)
        {
            T p;
            return submitWrapper<R>([p = std::move(smartPtr)]{return p->operator()();});
        }

        template<typename F>
        auto submit(F f)
        {
            return submitWrapper<void>(f);
        }

        template<typename R>
        auto submit(std::function<R()> f)
        {
            return submitWrapper<R>(f);
        }

        void stop() {
            done_ = true;
            std::cout << "Threads Pool size is " << threads_.size() << ". start stop threads..." << threadNum_-- << std::endl;
            submit<void>([this]{this->stop();});
        }

        void run_pending_task()
        {
            auto task = work_queue_->try_pop();
            if(task && !done_)
            {
                task->run();
            }
            else {
                throw EmptyPool();
            }
        }
    };
}

#endif //DEMO_THREADPOOL_H
