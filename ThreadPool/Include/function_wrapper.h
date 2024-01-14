//
// Created by sleepwalker on 2023/12/28.
//

#ifndef DEMO_FUNCTION_WRAPPER_H
#define DEMO_FUNCTION_WRAPPER_H

#include <memory>
#include <future>

namespace thread_pool
{
    class function_wrapper
    {
        struct impl_base
        {
            virtual void call() = 0;

            virtual ~impl_base() = default;
        };
        std::unique_ptr<impl_base> impl;
        template<typename F>
        struct impl_type : impl_base
        {
            F f;
            explicit impl_type(F &&f_) : f(std::move(f_)) {}
            void call() final {
                f();
            }
        };

    public:
        template<typename T>
        struct Is_packaged_task : std::false_type
        {
        };

        template<typename R>
        struct Is_packaged_task<std::packaged_task<R()>> : std::true_type
        {
        };

        template<typename F, typename = std::enable_if_t<Is_packaged_task<F>::value>>
        explicit function_wrapper(F &&f): impl(new impl_type(std::forward<F>(f))) {
        }

        function_wrapper() = default;
        function_wrapper(function_wrapper &&other) noexcept: impl(std::move(other.impl)) {}
        function_wrapper &operator=(function_wrapper &&other) noexcept {
            impl = std::move(other.impl);
            return *this;
        }
        function_wrapper(const function_wrapper &) = delete;
        function_wrapper(function_wrapper &) = delete;
        function_wrapper &operator=(const function_wrapper &) = delete;
        void run() { impl->call(); }
    };
}

#endif //DEMO_FUNCTION_WRAPPER_H
