//
// Created by sleepwalker on 2024/1/6.
//

#ifndef DEMO_QUICK_SORT_H
#define DEMO_QUICK_SORT_H

#include "../../ThreadPool/Include/ThreadPool.h"
#include "../../ThreadPool/Include/DataContainerFactory.h"
#include <list>

using namespace thread_pool;
namespace sort
{
    class QuickSort
    {
    public:
        explicit QuickSort(std::unique_ptr<DataContainerBase<function_wrapper>> container, int threadNum = 20) : pool_(
                std::move(container), threadNum) {};

        template<typename T>
        std::list<T> insert_sort(std::list<T> data) {
            if (data.empty())
            {
                return {};
            }
            std::list<T> result;
            result.splice(result.begin(), data, data.begin());
            for (auto i = data.begin(); i != data.end();)
            {
                auto d = std::move(*i);
                i = data.erase(i);
                auto j = result.end();
                j--;
                while (true)
                {
                    if (d >= *j)
                    {
                        result.insert(++j, std::move(d));
                        break;
                    }
                    if (j == result.begin())
                    {
                        result.insert(j, std::move(d));
                        break;
                    }
                    j--;
                }
            }
            return std::move(result);
        }

        template<typename T>
        std::list<T> do_normal_sort(std::list<T> data) {
            if (data.empty())
            {
                return data;
            }
            std::list<T> result;
            result.splice(result.end(), data, data.begin());
            auto pivot = *result.begin();
            auto divide_point = std::partition(data.begin(), data.end(), [&](auto &d) {
                return d < pivot;
            });
            std::list<T> lower_part;
            lower_part.splice(lower_part.begin(), data, data.begin(), divide_point);
            lower_part = do_normal_sort(std::move(lower_part));
            data = do_normal_sort(std::move(data));
            result.splice(result.begin(), lower_part);
            result.splice(result.end(), data);
            return std::move(result);
        }

        template<typename T>
        std::list<T> do_sort(std::list<T> data) {
            if (data.empty())
            {
                return data;
            }
            if (data.size() <= 500)
            {
                return do_normal_sort(std::move(data));
            }
            std::list<T> result;
            result.splice(result.begin(), data, data.begin());
            const T &pivot = *result.begin();
            auto divide_point = std::partition(data.begin(), data.end(), [&](const T &value) { return value < pivot; });
            std::list<T> lower_part;
            lower_part.splice(lower_part.end(), data, data.begin(), divide_point);
            auto res = pool_.submit<std::list<T>>(std::bind(&QuickSort::do_sort<T>, this, std::move(lower_part)));
            data = do_sort(std::move(data)); //will use rvalue
            result.splice(result.end(), data);
            while (res.wait_for(std::chrono::seconds(0)) == std::future_status::timeout)
            {
                if (!pool_.run_pending_task_v2())
                {
                    std::this_thread::yield();
                }
            }
            result.splice(result.begin(), res.get());
            return std::move(result);
        }

    private:
        ThreadPool pool_;
    };
}

#endif //DEMO_QUICK_SORT_H
