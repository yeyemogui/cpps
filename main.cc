#include <iostream>
#include "ThreadPool/Include/Functor.h"
#include "ThreadPool/Include/ThreadPool.h"
#include "ThreadPool/Include/DataContainerFactory.h"
#include <random>
#include "Sort/Include/quick_sort.h"
#include <chrono>
int main() {
    std::cout << "Hello, World!" << std::endl;
    sort::QuickSort sorter(11);
    std::list<int> data;
    std::default_random_engine  e;
    const auto total = 30000;
    std::uniform_int_distribution<int> u(0, total);
    auto start_time = std::chrono::high_resolution_clock::now();
    for(auto i = 0; i < total; i++)
    {
        data.push_back(u(e));
    }
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    std::cout << "used " << duration << " ms to create source data" << std::endl;


    auto res = sorter.do_sort(data);
    end_time = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    std::cout << "used " << duration << " ms to sort data" << std::endl;

    start_time = std::chrono::high_resolution_clock::now();
    data.sort();
    end_time = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    std::cout << "used " << duration << " ms to sort data" << std::endl;

    start_time = std::chrono::high_resolution_clock::now();
    return 0;
}
