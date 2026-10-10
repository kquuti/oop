#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "reposort/ArrayList.h"
#include "reposort/ComparatorChain.h"
#include "reposort/InsertionSort.h"
#include "reposort/LinkedList.h"
#include "reposort/MergeSort.h"
#include "reposort/QuickSort.h"
#include "reposort/domain/Commit.h"
#include "reposort/HeapSort.h"
#include "reposort/SelectionSort.h"
#include "reposort/ShellSort.h"

namespace {

// Тестові дані (порядок додавання важливий для перевірки стабільності):
//   id   автор  час  рядків
//   a1   Olena  100  10
//   a2   Ivan    50  200
//   a3   Olena   30  50
//   a4   Ivan    70  5
//   a5   Olena  100  80
void fillSample(IList<Commit>& list) {
    list.add(Commit("a1", "Olena", 100, "Fix bug", 10));
    list.add(Commit("a2", "Ivan", 50, "Add feature", 200));
    list.add(Commit("a3", "Olena", 30, "Refactor", 50));
    list.add(Commit("a4", "Ivan", 70, "Update docs", 5));
    list.add(Commit("a5", "Olena", 100, "Add tests", 80));
}

std::vector<std::string> ids(const IList<Commit>& list) {
    std::vector<std::string> result;
    for (std::size_t i = 0; i < list.size(); ++i) {
        result.push_back(list.get(i).id());
    }
    return result;
}

using Ids = std::vector<std::string>;

}  // namespace

// ---------- ComparatorChain на простих числах ----------

TEST(ComparatorChain, EmptyChainTreatsAllAsEqual) {
    ComparatorChain<int> chain;
    EXPECT_FALSE(chain(1, 2));
    EXPECT_FALSE(chain(2, 1));
    EXPECT_EQ(chain.size(), 0u);
}

TEST(ComparatorChain, FirstCriterionDecidesThenSecondBreaksTies) {
    // Спершу за парністю (парні перед непарними), потім за значенням.
    ComparatorChain<int> chain;
    chain.add(comparingBy<int>([](int x) { return x % 2; }))
         .add(naturalOrder<int>());
    EXPECT_EQ(chain.size(), 2u);

    EXPECT_TRUE(chain(2, 3));    // парне перед непарним
    EXPECT_FALSE(chain(3, 2));
    EXPECT_TRUE(chain(2, 4));    // однакова парність, 2 < 4
    EXPECT_FALSE(chain(4, 2));
    EXPECT_FALSE(chain(4, 4));   // повністю рівні
}

// ---------- Сортування комітів: усі три алгоритми ----------

template <typename SorterType>
class CommitSortTest : public ::testing::Test {};

using AllSorters =
    ::testing::Types<InsertionSort<Commit>, SelectionSort<Commit>, ShellSort<Commit>,
                     MergeSort<Commit>, QuickSort<Commit>, HeapSort<Commit>>;
TYPED_TEST_SUITE(CommitSortTest, AllSorters);

TYPED_TEST(CommitSortTest, ByLinesDescending) {
    TypeParam sorter;
    ArrayList<Commit> list;
    fillSample(list);
    sorter.sort(list, reversed(comparingBy<Commit>(&Commit::linesChanged)));
    EXPECT_EQ(ids(list), (Ids{"a2", "a5", "a3", "a1", "a4"}));
}

TYPED_TEST(CommitSortTest, ByAuthorThenTimestamp) {
    TypeParam sorter;
    ArrayList<Commit> list;
    fillSample(list);

    // Третій критерій (id) робить порядок повним, тож результат однаковий
    // навіть для нестабільного QuickSort.
    ComparatorChain<Commit> chain;
    chain.add(comparingBy<Commit>(&Commit::author))
         .add(comparingBy<Commit>(&Commit::timestamp))
         .add(comparingBy<Commit>(&Commit::id));

    sorter.sort(list, chain);
    EXPECT_EQ(ids(list), (Ids{"a2", "a4", "a3", "a1", "a5"}));
}

TYPED_TEST(CommitSortTest, ByAuthorAscendingThenLinesDescending) {
    TypeParam sorter;
    ArrayList<Commit> list;
    fillSample(list);

    ComparatorChain<Commit> chain;
    chain.add(comparingBy<Commit>(&Commit::author))
         .add(reversed(comparingBy<Commit>(&Commit::linesChanged)));

    sorter.sort(list, chain);
    EXPECT_EQ(ids(list), (Ids{"a2", "a4", "a5", "a3", "a1"}));
}

TYPED_TEST(CommitSortTest, WorksOnLinkedListToo) {
    TypeParam sorter;
    LinkedList<Commit> list;
    fillSample(list);
    sorter.sort(list, reversed(comparingBy<Commit>(&Commit::linesChanged)));
    EXPECT_EQ(ids(list), (Ids{"a2", "a5", "a3", "a1", "a4"}));
}

// ---------- Стабільність ----------

template <typename SorterType>
class StableSortTest : public ::testing::Test {};

using StableSorters = ::testing::Types<InsertionSort<Commit>, MergeSort<Commit>>;
TYPED_TEST_SUITE(StableSortTest, StableSorters);

// За одним автором коміти мають лишитись у початковому порядку.
TYPED_TEST(StableSortTest, EqualElementsKeepOriginalOrder) {
    TypeParam sorter;
    ArrayList<Commit> list;
    fillSample(list);
    sorter.sort(list, comparingBy<Commit>(&Commit::author));
    EXPECT_EQ(ids(list), (Ids{"a2", "a4", "a1", "a3", "a5"}));
}