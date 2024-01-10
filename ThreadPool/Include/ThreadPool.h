//
// Created by sleepwalker on 2023/12/27.
//

#ifndef DEMO_THREADPOOL_H
#define DEMO_THREADPOOL_H
#include "ThreadQueueLocked.h"
#include "function_wrapper.h"
#include <future>
#include <algorithm>
#include <execution>
#include "exceptions.h"
namespace thread_pool {
    class ThreadPool {
    private:
        std::unique_ptr<DataContainerBase<function_wrapper>> pool_data_container_;
        static thread_local DataContainerBase<function_wrapper>* local_data_container_;
        static thread_local int thread_index_;
        std::atomic<bool> done_ = false;
        std::vector <std::thread> threads_;
        std::atomic<bool> init_finished = false;
        std::vector<std::unique_ptr<DataContainerBase<function_wrapper>>> local_containers;
        unsigned int threadNum_;

        void worker(int threadIndex) {
            while (!init_finished)
                std::this_thread::yield();
            thread_index_ = threadIndex;
            local_data_container_ = local_containers[thread_index_].get();
            while (!done_) {
                if (!run_pending_task_v2())
                    std::this_thread::yield();
            }
        }

        template<typename R, typename F>
        auto submitWrapper(F f) {
            std::packaged_task < R() > task(std::move(f));
            std::future <R> res(task.get_future());
            if(thread_index_ != -1) //means not main thread
            {
                local_data_container_->push(function_wrapper(std::move(task)));
            }
            else //means main thread
            {
                pool_data_container_->push(function_wrapper(std::move(task)));
            }
            return res;
        }

        int find_heaviest_task()
        {
            if(!init_finished || threadNum_ == 0) //if no thread in the pool, just quit
            {
                return -1;
            }
            int index = 0;
            unsigned maxSize = local_containers[index]->size();
            for(auto threadIndex = 1; threadIndex < local_containers.size(); threadIndex++)
            {
                if(local_containers[threadIndex]->size() > maxSize)
                {
                    maxSize = local_containers[threadIndex]->size();
                    index = threadIndex;
                }
            }
            return index;
        }

        std::unique_ptr<function_wrapper> pop_from_local()
        {
            if(local_data_container_)
            {
                return local_data_container_->try_pop();
            }
            return nullptr;
        }

        std::unique_ptr<function_wrapper> pop_from_others()
        {
            auto heaviestThread = find_heaviest_task();
            if(heaviestThread != -1) {
                return local_containers[heaviestThread]->try_pop();
            }
            return nullptr;
        }

        std::unique_ptr<function_wrapper> pop_from_pool()
        {
            if(pool_data_container_) {
                return pool_data_container_->try_pop();
            }
            return nullptr;
        }

    public:
        explicit ThreadPool(std::unique_ptr<DataContainerBase<function_wrapper>> queue, unsigned int threadNum = 0) : pool_data_container_(std::move(queue)), done_(false) {
            threadNum_ = std::min(threadNum, std::thread::hardware_concurrency());
            try {
                for (int i = 0; i < threadNum_; i++) {
                    local_containers.push_back(pool_data_container_->clone());
                    threads_.emplace_back(&ThreadPool::worker, this, i);
                }
                init_finished = true;
            }
            catch (...) {
                stop();
                throw;
            }
        }

        ~ThreadPool() {
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
        auto submit(R (T::*f)(Types...), T object, Types... args) -> std::future<R> {
            auto func = std::bind(f, object, args...);
            return submitWrapper<R>(func);
        }

        template<typename R, typename T>
        auto submit(R (T::*f)(), T object) -> std::future<R> {
            return submitWrapper<R>([&object, f] { return (object.*f)(); });
        }

        template<typename T>
        struct isSmartPtr : public std::false_type{};
        template<typename T>
        struct isSmartPtr<std::unique_ptr<T>>: public std::true_type {};
        template<typename T>
        struct isSmartPtr<std::shared_ptr<T>>: public std::true_type {};

        template<typename R, typename T, typename = std::enable_if_t<isSmartPtr<T>::value>>
        auto submit(T smartPtr) -> std::future<R>
        {
            T p;
            return submitWrapper<R>([p = std::move(smartPtr)]{return p->operator()();});
        }

        template<typename F>
        auto submit(F f) -> std::future<void>
        {
            return submitWrapper<void>(f);
        }

        template<typename R>
        auto submit(std::function<R()> f) -> std::future<R>
        {
            return submitWrapper<R>(f);
        }

        void stop() {
            done_ = true;
            std::cout << "Threads Pool size is " << threads_.size() << ". start stop threads..." << threadNum_-- << std::endl;
            submit<void>([this]{this->stop();});
        }

        void run_pending_task_v1()
        {
            auto task = pool_data_container_->try_pop();
            if(task && !done_)
            {
                task->run();
            }
            else {
                throw EmptyPool();
            }
        }

        bool run_pending_task_v2()
        {
            if(done_)
            {
                return true;
            }
            std::unique_ptr<function_wrapper> task;
            if((task = pop_from_local()) || (task = pop_from_others()) || (task = pop_from_pool()))
            {
                task->run();
                return true;
            }
            return false;
        }
    };
    thread_local DataContainerBase<function_wrapper>* ThreadPool::local_data_container_ = nullptr;
    thread_local int ThreadPool::thread_index_ = -1;
}

#endif //DEMO_THREADPOOL_H
