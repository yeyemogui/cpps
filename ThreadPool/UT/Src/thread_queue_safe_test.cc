//
// Created by sleepwalker on 2023/12/29.
//
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../../Include/ThreadQueueLocked.h"
class thread_queue_safe_test: public testing::Test
{
protected:
    thread_pool::thread_queue::ThreadQueueLocked<int> m_queue;
};

TEST_F(thread_queue_safe_test, test_push)
{
    ASSERT_EQ(m_queue.size(), 0);
    for(int i = 1; i < 1000; i++)
    {
        m_queue.push((int)i);
        ASSERT_EQ(m_queue.size(), i);
    }
}

TEST_F(thread_queue_safe_test, test_pop)
{
    for(int i = 1; i < 1000; i++)
    {
        m_queue.push((int)i);
    }
    for(int i = 1; i < 1000; i++)
    {
        int a = *(m_queue.wait_and_pop());
        ASSERT_EQ(a, i);
        ASSERT_EQ(m_queue.size(), 999 - i);
    }
    ASSERT_EQ(m_queue.size(), 0);
}

TEST_F(thread_queue_safe_test, test_clear)
{
    for(int i = 1; i < 1000; i++)
    {
        m_queue.push((int)i);
    }
    ASSERT_EQ(m_queue.size(), 999);
    m_queue.clear();
    ASSERT_EQ(m_queue.size(), 0);
}

