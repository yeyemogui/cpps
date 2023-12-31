//
// Created by sleepwalker on 2023/12/29.
//
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../../Include/function_wrapper.h"
#include <future>
using namespace thread_pool;
class MockFunc
{
public:
    //MOCK_METHOD0(call, int());
    MockFunc(): i(5){};
    int call()
    {
        i = 100;
        return i;
    }
    int i;
};

TEST(function_wrapper_test, test_run)
{
    MockFunc mockFunc;
    auto t = [&mockFunc]{return mockFunc.call();};
    //EXPECT_CALL(mockFunc, call()).Times(1).WillRepeatedly(::testing::Return(++i));
    std::packaged_task<int()> task(t);
    function_wrapper wrapper(std::move(task));
    ASSERT_EQ(mockFunc.i, 5);
    wrapper.run();
    ASSERT_EQ(mockFunc.i, 100);
}
