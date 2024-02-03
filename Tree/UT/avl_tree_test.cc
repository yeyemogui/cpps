//
// Created by sleepwalker on 2024/1/21.
//
#include "../Include/AvlTree.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <random>
using namespace trees;
TEST(avl_tree_test, Insert_test)
{
    AvlTree<int> tree;
    tree.Insert(6);
    tree.Insert(5);
    tree.Insert(7);
    ASSERT_NE(tree.Find(5), nullptr);
    ASSERT_NE(tree.Find(6), nullptr);
    ASSERT_NE(tree.Find(7), nullptr);
}

TEST(avl_tree_test, Find_test)
{
    AvlTree<int> tree;
    tree.Insert(6);
    tree.Insert(5);
    tree.Insert(7);
    auto data = tree.Find(5);
    ASSERT_EQ(*data, 5);
    data = tree.Find(6);
    ASSERT_EQ(*data, 6);
    data = tree.Find(7);
    ASSERT_EQ(*data, 7);
}

TEST(avl_tree_test, Delete_test)
{
    AvlTree<int> tree;
    tree.Insert(6);
    tree.Insert(5);
    tree.Insert(7);
    tree.Insert(8);
    tree.Insert(4);
    tree.Delete(8);
    auto data = tree.Find(8);
    ASSERT_EQ(data, nullptr);
    data = tree.Find(5);
    ASSERT_EQ(*data, 5);
    tree.Delete(5);
    std::vector<int> dst{4,6,7,};
    auto sorted = tree.GetSortedData();
    ASSERT_EQ(sorted.size(), dst.size());
    auto s = sorted.begin();
    auto d = dst.begin();
    while(s != sorted.end() && d != dst.end())
    {
        ASSERT_EQ(*s, *d);
        s++;
        d++;
    }
}

TEST(avl_tree_test, GetSortedData_test)
{
    AvlTree<int> tree;
    const auto total = 500;
    std::vector<int> data;
    std::default_random_engine  e;
    std::uniform_int_distribution<int> u(0, total);
    for(auto i = 0; i < total; i++)
    {
        auto d = u(e);
        data.push_back(d);
        tree.Insert(d);
    }
    auto sorted = tree.GetSortedData();
    std::sort(data.begin(), data.end());
    ASSERT_EQ(sorted.size(), data.size());
    auto s = sorted.begin();
    auto d = data.begin();
    while(s != sorted.end() && d != data.end())
    {
        ASSERT_EQ(*s, *d);
        s++;
        d++;
    }
}