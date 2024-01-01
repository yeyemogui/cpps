#include <iostream>
#include "ThreadPool/Include/Functor.h"
#include "ThreadPool/Include/threadPool.h"
#include "ThreadPool/Include/thread_queue_factory.h"
#include <thread>
int main() {
    std::cout << "Hello, World!" << std::endl;
    class test
    {
    public:
        int print(int i)
        {
            std::cout << "start print " << i << " ..." << std::endl;
            return i;
        }
    };
    thread_pool::threadPool pool(thread_pool::thread_queue::thread_queue_factory::create_safe_queue(), 10);
    test tt;
    int i = 1;
    while(i < 999999)
    {
        pool.submit(&test::print, tt, i++);
    }
    auto res = pool.submit(&test::print,tt, i);
    res.get();
    return 0;
}
