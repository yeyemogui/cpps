//
// Created by sleepwalker on 2024/1/6.
//
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../../Include/quick_sort.h"
#include <random>
TEST(quick_sort_test, do_sort_test)
{
    sort::QuickSort sorter;
    const auto total = 500000;
    std::list<int> data;
    std::default_random_engine  e;
    std::uniform_int_distribution<int> u(0, total);
    for(auto i = 0; i < total; i++)
    {
        data.push_back(u(e));
    }
    std::cout << std::endl;
    auto res = sorter.do_sort(data);
    ASSERT_EQ(data.size(), res.size());
    data.sort();
    auto d = data.begin();
    auto r = res.begin();
    while(d != data.end() && r != res.end())
    {
        ASSERT_EQ(*d, *r);
        d++;
        r++;
    }
    std::cout << std::endl;
}

TEST(quick_sort_test, insert_sort_test)
{
    sort::QuickSort sorter;
    const auto total = 100;
    std::list<int> data;
    std::default_random_engine  e;
    std::uniform_int_distribution<int> u(0, total);
    for(auto i = 0; i < total; i++)
    {
        data.push_back(u(e));
    }
    std::cout << std::endl;
    auto res = sorter.insert_sort(data);
    ASSERT_EQ(data.size(), res.size());
    data.sort();
    auto d = data.begin();
    auto r = res.begin();
    while(d != data.end() && r != res.end())
    {
        ASSERT_EQ(*d, *r);
        d++;
        r++;
    }
    std::cout << std::endl;
}

TEST(quick_sort_test, normal_sort_test)
{
    sort::QuickSort sorter;
    const auto total = 100;
    std::list<int> data;
    std::default_random_engine  e;
    std::uniform_int_distribution<int> u(0, total);
    for(auto i = 0; i < total; i++)
    {
        data.push_back(u(e));
    }
    std::cout << std::endl;
    auto res = sorter.do_normal_sort(data);
    ASSERT_EQ(data.size(), res.size());
    data.sort();
    auto d = data.begin();
    auto r = res.begin();
    while(d != data.end() && r != res.end())
    {
        ASSERT_EQ(*d, *r);
        d++;
        r++;
    }
    std::cout << std::endl;
}