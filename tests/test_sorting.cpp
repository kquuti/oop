#include <gtest/gtest.h>

#include <algorithm>
#include <memory>
#include <random>
#include <string>
#include <vector>

#include "reposort/ArrayList.h"
#include "reposort/InsertionSort.h"
#include "reposort/LinkedList.h"
#include "reposort/MergeSort.h"
#include "reposort/QuickSort.h"
#include "reposort/HeapSort.h"
#include "reposort/SelectionSort.h"
#include "reposort/ShellSort.h"

namespace {

std::vector<int> toVector(const IList<int>& list) {
    std::vector<int> result;
    for (std::size_t i = 0; i < list.size(); ++i) {
        result.push_back(list.get(i));
    }
    return result;
}

// Сортує вхідні дані заданим алгоритмом у списку заданого типу.
template <typename ListType>
std::vector<int> sortedBy(const ISortStrategy<int>& sorter,
                          const std::vector<int>& input,
                          const Comparator<int>& cmp) {
    ListType list;
    for (int value : input) {
        list.add(value);
    }
    sorter.sort(list, cmp);
    return toVector(list);
}

// Перевіряє результат на обох типах списків проти std::sort.
void expectSortsCorrectly(const ISortStrategy<int>& sorter,
                          const std::vector<int>& input) {
    std::vector<int> ascending = input;
    std::sort(ascending.begin(), ascending.end());

    EXPECT_EQ((sortedBy<ArrayList<int>>(sorter, input, naturalOrder<int>())), ascending);
    EXPECT_EQ((sortedBy<LinkedList<int>>(sorter, input, naturalOrder<int>())), ascending);

    std::vector<int> descending = input;
    std::sort(descending.begin(), descending.end(), std::greater<int>());

    const Comparator<int> desc = reversed(naturalOrder<int>());
    EXPECT_EQ((sortedBy<ArrayList<int>>(sorter, input, desc)), descending);
    EXPECT_EQ((sortedBy<LinkedList<int>>(sorter, input, desc)), descending);
}

}  // namespace

// Усі тести запускаються для кожного алгоритму.
template <typename SorterType>
class SortTest : public ::testing::Test {};

using SorterTypes = ::testing::Types<InsertionSort<int>, SelectionSort<int>, ShellSort<int>,
                                     MergeSort<int>, QuickSort<int>, HeapSort<int>>;
TYPED_TEST_SUITE(SortTest, SorterTypes);

TYPED_TEST(SortTest, EmptyList) {
    TypeParam sorter;
    expectSortsCorrectly(sorter, {});
}

TYPED_TEST(SortTest, SingleElement) {
    TypeParam sorter;
    expectSortsCorrectly(sorter, {42});
}

TYPED_TEST(SortTest, TwoElements) {
    TypeParam sorter;
    expectSortsCorrectly(sorter, {2, 1});
}

TYPED_TEST(SortTest, AlreadySorted) {
    TypeParam sorter;
    expectSortsCorrectly(sorter, {1, 2, 3, 4, 5, 6, 7});
}

TYPED_TEST(SortTest, ReverseSorted) {
    TypeParam sorter;
    expectSortsCorrectly(sorter, {7, 6, 5, 4, 3, 2, 1});
}

TYPED_TEST(SortTest, WithDuplicates) {
    TypeParam sorter;
    expectSortsCorrectly(sorter, {3, 1, 3, 2, 1, 3, 2});
}

TYPED_TEST(SortTest, AllEqual) {
    TypeParam sorter;
    expectSortsCorrectly(sorter, {5, 5, 5, 5, 5});
}

TYPED_TEST(SortTest, NegativeNumbers) {
    TypeParam sorter;
    expectSortsCorrectly(sorter, {0, -3, 8, -10, 5, -1});
}

TYPED_TEST(SortTest, RandomData) {
    TypeParam sorter;
    std::mt19937 generator(12345);  // фіксоване зерно: тест завжди однаковий
    std::uniform_int_distribution<int> distribution(-1000, 1000);
    for (int round = 0; round < 20; ++round) {
        std::vector<int> input;
        const int size = round * 7;  // від 0 до 133 елементів
        for (int i = 0; i < size; ++i) {
            input.push_back(distribution(generator));
        }
        expectSortsCorrectly(sorter, input);
    }
}

TYPED_TEST(SortTest, SortsStrings) {
    TypeParam unused;
    (void)unused;
    InsertionSort<std::string> insertion;
    MergeSort<std::string> merge;
    QuickSort<std::string> quick;
    const std::vector<const ISortStrategy<std::string>*> sorters = {&insertion, &merge, &quick};
    for (const auto* sorter : sorters) {
        ArrayList<std::string> list;
        list.add("pear");
        list.add("apple");
        list.add("fig");
        sorter->sort(list, naturalOrder<std::string>());
        EXPECT_EQ(list.get(0), "apple");
        EXPECT_EQ(list.get(1), "fig");
        EXPECT_EQ(list.get(2), "pear");
    }
}

// Клієнтський код вибирає алгоритм і працює лише через інтерфейс.
TEST(Sorting, WorksThroughStrategyInterface) {
    std::vector<std::unique_ptr<ISortStrategy<int>>> sorters;
    sorters.push_back(std::make_unique<InsertionSort<int>>());
    sorters.push_back(std::make_unique<MergeSort<int>>());
    sorters.push_back(std::make_unique<QuickSort<int>>());

    for (const auto& sorter : sorters) {
        ArrayList<int> list;
        list.add(3);
        list.add(1);
        list.add(2);
        sorter->sort(list, naturalOrder<int>());
        EXPECT_EQ(toVector(list), (std::vector<int>{1, 2, 3}));
    }
}

TEST(Sorting, AlgorithmsHaveNames) {
    EXPECT_EQ(InsertionSort<int>().name(), "Insertion sort");
    EXPECT_EQ(MergeSort<int>().name(), "Merge sort");
    EXPECT_EQ(QuickSort<int>().name(), "Quick sort");
}