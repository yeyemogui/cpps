//
// Created by sleepwalker on 2024/1/6.
//

#ifndef THREADPOOLTEST_EXCEPTIONS_H
#define THREADPOOLTEST_EXCEPTIONS_H
#include <exception>
class EmptyPool: public std::exception
{
public:
    const char* what() noexcept
    {
        return "the pool is empty";
    }
};

class UnsupportedOperation: public std::exception
{
public:
    const char* what() noexcept
    {
        return "the operation is not supported";
    }
};

class InternalContainerErr: public std::exception
{
public:
    const char* what() noexcept
    {
        return "there is internal error happened within container...";
    }
};
#endif //THREADPOOLTEST_EXCEPTIONS_H
