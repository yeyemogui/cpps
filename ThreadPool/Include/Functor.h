//
// Created by sleepwalker on 2023/12/28.
//

#ifndef DEMO_FUNCTOR_H
#define DEMO_FUNCTOR_H
#include <utility>
template<typename T, typename R>
class Functor {
public:
    typedef R (T::*function)();
    Functor(function f, T& obj): object(obj), f(f)
    {

    };

    R operator()()
    {
        return (object.*f)();
    }
private:
    function f;
    T& object;

};


#endif //DEMO_FUNCTOR_H
