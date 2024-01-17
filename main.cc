#include <iostream>
#include "ThreadPool/Include/ThreadPool.h"
#include "ThreadPool/Include/DataContainerFactory.h"
#include <random>
#include "Sort/Include/quick_sort.h"
#include <chrono>
#include <fstream>


int main() {
    std::cout << "Hello, World!" << std::endl;
    std::ofstream out("statistic.txt", std::ios::out);
    for (auto dd = 0; dd < 2; dd++)
    {
        std::cout << "Begin============================================Begin" << std::endl;
        std::list<int> data;
        std::default_random_engine e;
        const auto total = 1000000;
        std::uniform_int_distribution<int> u(0, total);
        auto start_time = std::chrono::high_resolution_clock::now();
        for (auto i = 0; i < total; i++)
        {
            data.push_back(u(e));
        }
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
        std::cout << "used " << duration << " ms to generate source data" << std::endl;

        {
            sort::QuickSort sorter_locked_queue(DataContainerFactory::create_stack_locked(), 0);
            start_time = std::chrono::high_resolution_clock::now();
            auto res = sorter_locked_queue.do_normal_sort(data);
            end_time = std::chrono::high_resolution_clock::now();
            duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
            std::cout << "Standard Quick Sort used " << duration << " ms to sort data" << std::endl;
            out << duration << " ";
        }

        /* has bug, still work....
        {
            sort::QuickSort sorter_stack_lock_free(DataContainerFactory::create_stack_lock_freeV1(), 11);
            start_time = std::chrono::high_resolution_clock::now();
            auto res = sorter_stack_lock_free.do_sort(data);
            end_time = std::chrono::high_resolution_clock::now();
            duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
            std::cout << "Lock-Free Stack V1 used " << duration << " ms to sort data" << std::endl;
            out << duration << " ";
        }*/

        {
            sort::QuickSort sorter_stack_lock_free(DataContainerFactory::create_stack_lock_freeV2(), 11);
            start_time = std::chrono::high_resolution_clock::now();
            auto res = sorter_stack_lock_free.do_sort(data);
            end_time = std::chrono::high_resolution_clock::now();
            duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
            std::cout << "Lock-Free Stack V2 used " << duration << " ms to sort data" << std::endl;
            out << duration << " ";
        }


        {
            sort::QuickSort sorter_queue_lock_free(DataContainerFactory::create_stack_lock_freeV3(), 11);
            start_time = std::chrono::high_resolution_clock::now();
            auto res = sorter_queue_lock_free.do_sort(data);
            end_time = std::chrono::high_resolution_clock::now();
            duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
            std::cout << "Lock-Free Stack V3 used " << duration << " ms to sort data" << std::endl;
            out << duration << " ";
        }

        {
            sort::QuickSort sorter_queue_lock_free(DataContainerFactory::create_queue_lock_freeV2(), 11);
            start_time = std::chrono::high_resolution_clock::now();
            auto res = sorter_queue_lock_free.do_sort(data);
            end_time = std::chrono::high_resolution_clock::now();
            duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
            std::cout << "Lock-Free Queue V2 used " << duration << " ms to sort data" << std::endl;
            out << duration << " ";
        }
/* this one has bug, still debug
        {
            sort::QuickSort sorter_queue_lock_free(DataContainerFactory::create_queue_lock_free(), 11);
            start_time = std::chrono::high_resolution_clock::now();
            auto res = sorter_queue_lock_free.do_sort(data);
            end_time = std::chrono::high_resolution_clock::now();
            duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
            std::cout << "Lock-Free Queue used " << duration << " ms to sort data" << std::endl;
            out << duration << " ";
        }
*/
        {
            sort::QuickSort sorter_locked_stack(DataContainerFactory::create_stack_locked(), 11);
            start_time = std::chrono::high_resolution_clock::now();
            auto res = sorter_locked_stack.do_sort(data);
            end_time = std::chrono::high_resolution_clock::now();
            duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
            std::cout << "Locked Stack used " << duration << " ms to sort data" << std::endl;
            out << duration << " ";
        }

        {
            sort::QuickSort sorter_locked_queue(DataContainerFactory::create_safe_queue(), 11);
            start_time = std::chrono::high_resolution_clock::now();
            auto res = sorter_locked_queue.do_sort(data);
            end_time = std::chrono::high_resolution_clock::now();
            duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
            std::cout << "Locked Queue used " << duration << " ms to sort data" << std::endl;
            out << duration << " ";
        }




        start_time = std::chrono::high_resolution_clock::now();
        data.sort();
        end_time = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
        std::cout << "std::sort used " << duration << " ms to sort data" << std::endl;
        out << duration << " ";
        out << std::endl;
        std::cout << "End============================================End" << std::endl;
        std::cout << std::endl;
    }
    out.close();
    return 0;
}
