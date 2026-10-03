#include <gtest/gtest.h>

#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "reposort/ArrayList.h"
#include "reposort/LinkedList.h"
#include "reposort/VectorList.h"

// Усі тести нижче запускаються по одному разу для кожної реалізації списку.
template <typename ListType>
class ListTest : public ::testing::Test {};

using ListTypes = ::testing::Types<VectorList<int>, ArrayList<int>, LinkedList<int>>;
TYPED_TEST_SUITE(ListTest, ListTypes);

TYPED_TEST(ListTest, NewListIsEmpty) {
    TypeParam list;
    EXPECT_EQ(list.size(), 0u);
    EXPECT_TRUE(list.isEmpty());
}

TYPED_TEST(ListTest, AddKeepsOrder) {
    TypeParam list;
    list.add(10);
    list.add(20);
    list.add(30);
    EXPECT_EQ(list.size(), 3u);
    EXPECT_EQ(list.get(0), 10);
    EXPECT_EQ(list.get(1), 20);
    EXPECT_EQ(list.get(2), 30);
}

TYPED_TEST(ListTest, SetReplacesItem) {
    TypeParam list;
    list.add(1);
    list.set(0, 99);
    EXPECT_EQ(list.get(0), 99);
}

TYPED_TEST(ListTest, RemoveFirst) {
    TypeParam list;
    list.add(1);
    list.add(2);
    list.add(3);
    list.removeAt(0);
    EXPECT_EQ(list.size(), 2u);
    EXPECT_EQ(list.get(0), 2);
    EXPECT_EQ(list.get(1), 3);
}

TYPED_TEST(ListTest, RemoveMiddle) {
    TypeParam list;
    list.add(1);
    list.add(2);
    list.add(3);
    list.removeAt(1);
    EXPECT_EQ(list.size(), 2u);
    EXPECT_EQ(list.get(0), 1);
    EXPECT_EQ(list.get(1), 3);
}

TYPED_TEST(ListTest, RemoveLast) {
    TypeParam list;
    list.add(1);
    list.add(2);
    list.add(3);
    list.removeAt(2);
    EXPECT_EQ(list.size(), 2u);
    EXPECT_EQ(list.get(1), 2);
}

TYPED_TEST(ListTest, RemoveOnlyElementLeavesEmptyList) {
    TypeParam list;
    list.add(1);
    list.removeAt(0);
    EXPECT_TRUE(list.isEmpty());
}

// Перевіряє, що після видалення останнього елемента додавання працює правильно
// (у LinkedList для цього має оновитися tail_).
TYPED_TEST(ListTest, AddAfterRemovingLast) {
    TypeParam list;
    list.add(1);
    list.add(2);
    list.removeAt(1);
    list.add(3);
    EXPECT_EQ(list.size(), 2u);
    EXPECT_EQ(list.get(0), 1);
    EXPECT_EQ(list.get(1), 3);
}

TYPED_TEST(ListTest, SwapExchangesItems) {
    TypeParam list;
    list.add(1);
    list.add(2);
    list.swap(0, 1);
    EXPECT_EQ(list.get(0), 2);
    EXPECT_EQ(list.get(1), 1);
}

TYPED_TEST(ListTest, WrongIndexThrows) {
    TypeParam list;
    list.add(1);
    EXPECT_THROW(list.get(1), std::out_of_range);
    EXPECT_THROW(list.set(5, 0), std::out_of_range);
    EXPECT_THROW(list.removeAt(2), std::out_of_range);
}

TYPED_TEST(ListTest, EmptyListAccessThrows) {
    TypeParam list;
    EXPECT_THROW(list.get(0), std::out_of_range);
    EXPECT_THROW(list.removeAt(0), std::out_of_range);
}

// Для ArrayList це перевіряє розширення масиву (кілька разів).
TYPED_TEST(ListTest, ManyElements) {
    TypeParam list;
    for (int i = 0; i < 100; ++i) {
        list.add(i);
    }
    EXPECT_EQ(list.size(), 100u);
    for (int i = 0; i < 100; ++i) {
        EXPECT_EQ(list.get(static_cast<std::size_t>(i)), i);
    }
}

// Списки працюють не лише з int.
TEST(Lists, WorkWithStrings) {
    ArrayList<std::string> arrayList;
    LinkedList<std::string> linkedList;
    arrayList.add("abc");
    linkedList.add("xyz");
    EXPECT_EQ(arrayList.get(0), "abc");
    EXPECT_EQ(linkedList.get(0), "xyz");
}

// Клієнтський код працює через інтерфейс і не знає, який саме список всередині.
TEST(Lists, WorkThroughCommonInterface) {
    std::vector<std::unique_ptr<IList<int>>> lists;
    lists.push_back(std::make_unique<VectorList<int>>());
    lists.push_back(std::make_unique<ArrayList<int>>());
    lists.push_back(std::make_unique<LinkedList<int>>());

    for (auto& list : lists) {
        list->add(5);
        list->add(6);
        EXPECT_EQ(list->get(1), 6);
    }
}