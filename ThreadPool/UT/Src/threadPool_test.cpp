//
// Created by sleepwalker on 2023/12/30.
//
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../../Include/thread_queue_base.h"
#include "../../Include/function_wrapper.h"
#include "../../Include/threadPool.h"
#include "../../Include/thread_queue_factory.h"
#include "../../Include/exceptions.h"

using namespace thread_pool;
class MockTask
{
public:
    MockTask() = default;
    MockTask(const MockTask& other) {};
    MOCK_METHOD0(run, int());
};

class MockTask2
{
public:
    int operator()()
    {
        return 50;
    }
};

TEST(thread_pool_test, test_submit_objMethod)
{
    auto queue = thread_queue::thread_queue_factory::create_safe_queue();
    thread_pool::threadPool pool(std::move(queue), 10);
    MockTask task;
    EXPECT_CALL(task, run()).Times(1).WillRepeatedly(::testing::Return(5));
    auto res = pool.submit<int>([&task]{return task.run();});
    ASSERT_EQ(res.get(), 5);
}

TEST(thread_pool_test, test_submit_smartPtr)
{
    auto queue = thread_queue::thread_queue_factory::create_safe_queue();
    thread_pool::threadPool pool(std::move(queue), 10);
    auto p = std::make_unique<MockTask2>();
    auto res = pool.submit<int>(std::move(p));
    ASSERT_EQ(res.get(), 50);
}

TEST(thread_pool_test, test_run_pending_task)
{
    auto queue = thread_queue::thread_queue_factory::create_safe_queue();
    thread_pool::threadPool pool(std::move(queue), 0);
    auto p = std::make_unique<MockTask2>();
    auto res = pool.submit<int>(std::move(p));
    pool.run_pending_task();
    ASSERT_EQ(res.get(), 50);
}

TEST(thread_pool_test, test_run_pending_task_exception)
{
    auto queue = thread_queue::thread_queue_factory::create_safe_queue();
    thread_pool::threadPool pool(std::move(queue), 0);
    auto p = std::make_unique<MockTask2>();
    auto res = pool.submit<int>(std::move(p));
    pool.run_pending_task();
    ASSERT_EQ(res.get(), 50);
    ASSERT_THROW(pool.run_pending_task(), EmptyPool);
}