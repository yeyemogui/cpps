//
// Created by sleepwalker on 2023/12/30.
//
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../../Include/DataContainerBase.h"
#include "../../Include/function_wrapper.h"
#include "../../Include/ThreadPool.h"
#include "../../Include/DataContainerFactory.h"
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
    auto queue = DataContainerFactory::create_safe_queue();
    thread_pool::ThreadPool pool(std::move(queue), 10);
    MockTask task;
    EXPECT_CALL(task, run()).Times(1).WillRepeatedly(::testing::Return(5));
    auto res = pool.submit<int>([&task]{return task.run();});
    ASSERT_EQ(res.get(), 5);
}

TEST(thread_pool_test, test_submit_smartPtr)
{
    auto queue = DataContainerFactory::create_safe_queue();
    thread_pool::ThreadPool pool(std::move(queue), 10);
    auto p = std::make_unique<MockTask2>();
    auto res = pool.submit<int>(std::move(p));
    ASSERT_EQ(res.get(), 50);
}

TEST(thread_pool_test, test_run_pending_task)
{
    auto queue = DataContainerFactory::create_safe_queue();
    thread_pool::ThreadPool pool(std::move(queue), 0);
    auto p = std::make_unique<MockTask2>();
    auto res = pool.submit<int>(std::move(p));
    pool.run_pending_task_v1();
    ASSERT_EQ(res.get(), 50);
}

TEST(thread_pool_test, test_run_pending_task_exception)
{
    auto queue = DataContainerFactory::create_queue_lock_free();
    thread_pool::ThreadPool pool(std::move(queue), 0);
    auto p = std::make_unique<MockTask2>();
    auto res = pool.submit<int>(std::move(p));
    auto ret = false;
    while(!ret)
    {
        ret = pool.run_pending_task_v2();
    }
    ASSERT_EQ(res.get(), 50);
    ASSERT_EQ(ret, true);
    ASSERT_THROW(pool.run_pending_task_v1(), EmptyPool);
    ret = pool.run_pending_task_v2();
    ASSERT_EQ(ret, false);
}