//
// Created by sleepwalker on 2023/12/30.
//

#ifndef DEMO_DATACONTAINERBASE_H
#define DEMO_DATACONTAINERBASE_H

#include <memory>

namespace thread_pool
{
    template<typename T>
    class DataContainerBase
    {
    public:
        virtual void push(T &&) = 0;

        virtual void clear() = 0;

        virtual unsigned int size() = 0;

        virtual std::unique_ptr<T> wait_and_pop() = 0;

        virtual std::unique_ptr<T> try_pop() = 0;

        virtual std::unique_ptr<DataContainerBase<T>> clone() = 0;

        virtual ~DataContainerBase() = default;
    };

}
#endif //DEMO_DATACONTAINERBASE_H