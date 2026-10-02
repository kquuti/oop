#include <gtest/gtest.h>

#include <memory>
#include <stdexcept>
#include <string>

#include "reposort/VectorList.h"

TEST(VectorList, NewListIsEmpty) {
    VectorList<int> list;
    EXPECT_EQ(list.size(), 0u);
    EXPECT_TRUE(list.isEmpty());
}

TEST(VectorList, AddIncreasesSizeAndKeepsOrder) {
    VectorList<int> list;
    list.add(10);
    list.add(20);
    EXPECT_EQ(list.size(), 2u);
    EXPECT_EQ(list.get(0), 10);
    EXPECT_EQ(list.get(1), 20);
}

TEST(VectorList, SetReplacesItem) {
    VectorList<int> list;
    list.add(1);
    list.set(0, 99);
    EXPECT_EQ(list.get(0), 99);
}

TEST(VectorList, RemoveAtShiftsItems) {
    VectorList<int> list;
    list.add(1);
    list.add(2);
    list.add(3);
    list.removeAt(1);
    EXPECT_EQ(list.size(), 2u);
    EXPECT_EQ(list.get(0), 1);
    EXPECT_EQ(list.get(1), 3);
}

TEST(VectorList, SwapExchangesItems) {
    VectorList<int> list;
    list.add(1);
    list.add(2);
    list.swap(0, 1);
    EXPECT_EQ(list.get(0), 2);
    EXPECT_EQ(list.get(1), 1);
}

TEST(VectorList, WrongIndexThrows) {
    VectorList<int> list;
    list.add(1);
    EXPECT_THROW(list.get(1), std::out_of_range);
    EXPECT_THROW(list.set(5, 0), std::out_of_range);
    EXPECT_THROW(list.removeAt(2), std::out_of_range);
}

TEST(VectorList, WorksWithStrings) {
    VectorList<std::string> list;
    list.add("abc");
    EXPECT_EQ(list.get(0), "abc");
}

// Клієнтський код працює через інтерфейс, не знаючи конкретного типу списку.
TEST(VectorList, WorksThroughInterface) {
    std::unique_ptr<IList<int>> list = std::make_unique<VectorList<int>>();
    list->add(7);
    EXPECT_EQ(list->get(0), 7);
}