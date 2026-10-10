#include <gtest/gtest.h>

#include <algorithm>
#include <cstdint>
#include <random>
#include <string>
#include <vector>

#include "reposort/ArrayList.h"
#include "reposort/LinkedList.h"
#include "reposort/RandomData.h"
#include "reposort/SortRegistry.h"
#include "reposort/StandardSorts.h"
#include "reposort/VectorList.h"
#include "reposort/domain/Commit.h"
#include "reposort/domain/RandomCommit.h"

namespace {

template <typename T>
std::vector<T> toStdVector(const IList<T>& list) {
    std::vector<T> result;
    for (std::size_t i = 0; i < list.size(); ++i) {
        result.push_back(list.get(i));
    }
    return result;
}

// Заповнює список випадковими даними, сортує кожним зареєстрованим алгоритмом
// і порівнює з результатом std::sort. Порівняння йде через текстове подання,
// бо для Commit не визначено operator==.
template <typename T>
void expectAllAlgorithmsSortRandomData() {
    SortRegistry<T> registry;
    registerStandardSorts(registry);
    for (const std::string& key : registry.keys()) {
        std::mt19937 generator(2025);
        ArrayList<T> list;
        fillRandom(list, 50, generator);

        std::vector<T> expected = toStdVector<T>(list);
        std::sort(expected.begin(), expected.end());

        registry.create(key)->sort(list, naturalOrder<T>());
        EXPECT_EQ(toText(toStdVector<T>(list)), toText(expected)) << "algorithm: " << key;
    }
}

}  // namespace

// ---------- toText ----------

TEST(TextFormat, NumbersAndStrings) {
    EXPECT_EQ(toText(42), "42");
    EXPECT_EQ(toText(1.5), "1.5");
    EXPECT_EQ(toText(2.0), "2");
    EXPECT_EQ(toText(std::string("abc")), "abc");
}

TEST(TextFormat, Vectors) {
    EXPECT_EQ(toText(std::vector<int>{}), "[]");
    EXPECT_EQ(toText(std::vector<int>{1, 2, 3}), "[1, 2, 3]");
    EXPECT_EQ(toText(std::vector<std::vector<int>>{{1, 2}, {3}}), "[[1, 2], [3]]");
}

TEST(TextFormat, CommitUsesToString) {
    Commit commit("a1", "Olena", 100, "Fix bug", 10);
    EXPECT_EQ(toText(commit), commit.toString());
}

// ---------- IList::toString ----------

template <typename ListType>
class ListToStringTest : public ::testing::Test {};

using IntLists = ::testing::Types<VectorList<int>, ArrayList<int>, LinkedList<int>>;
TYPED_TEST_SUITE(ListToStringTest, IntLists);

TYPED_TEST(ListToStringTest, EmptyList) {
    TypeParam list;
    EXPECT_EQ(list.toString(), "[]");
}

TYPED_TEST(ListToStringTest, SeveralItems) {
    TypeParam list;
    list.add(1);
    list.add(2);
    list.add(3);
    EXPECT_EQ(list.toString(), "[1, 2, 3]");
}

TEST(ListToString, Strings) {
    ArrayList<std::string> list;
    list.add("abc");
    list.add("xyz");
    EXPECT_EQ(list.toString(), "[abc, xyz]");
}

TEST(ListToString, Doubles) {
    ArrayList<double> list;
    list.add(1.5);
    list.add(2.0);
    EXPECT_EQ(list.toString(), "[1.5, 2]");
}

TEST(ListToString, Vectors) {
    ArrayList<std::vector<int>> list;
    list.add(std::vector<int>{1, 2});
    list.add(std::vector<int>{3});
    EXPECT_EQ(list.toString(), "[[1, 2], [3]]");
}

TEST(ListToString, Commits) {
    ArrayList<Commit> list;
    list.add(Commit("a1", "Olena", 100, "Fix bug", 10));
    EXPECT_EQ(list.toString(), "[" + list.get(0).toString() + "]");
}

// ---------- Commit: природний порядок ----------

TEST(Commit, NaturalOrderIsChronological) {
    Commit early("b", "x", 10, "m", 1);
    Commit late("a", "x", 20, "m", 1);
    EXPECT_TRUE(early < late);
    EXPECT_FALSE(late < early);
}

TEST(Commit, NaturalOrderUsesIdForSameTime) {
    Commit first("a", "x", 10, "m", 1);
    Commit second("b", "x", 10, "m", 1);
    EXPECT_TRUE(first < second);
    EXPECT_FALSE(second < first);
    EXPECT_FALSE(first < first);
}

// ---------- Випадкові дані ----------

TEST(RandomData, SameSeedGivesSameData) {
    std::mt19937 firstGenerator(42);
    std::mt19937 secondGenerator(42);
    ArrayList<int> first;
    ArrayList<int> second;
    fillRandom(first, 20, firstGenerator);
    fillRandom(second, 20, secondGenerator);
    EXPECT_EQ(first.toString(), second.toString());
}

TEST(RandomData, DifferentSeedsGiveDifferentData) {
    std::mt19937 firstGenerator(1);
    std::mt19937 secondGenerator(2);
    ArrayList<int> first;
    ArrayList<int> second;
    fillRandom(first, 20, firstGenerator);
    fillRandom(second, 20, secondGenerator);
    EXPECT_NE(first.toString(), second.toString());
}

TEST(RandomData, FillsRequestedCount) {
    std::mt19937 generator(1);
    ArrayList<int> list;
    fillRandom(list, 10, generator);
    EXPECT_EQ(list.size(), 10u);

    ArrayList<int> empty;
    fillRandom(empty, 0, generator);
    EXPECT_TRUE(empty.isEmpty());
}

TEST(RandomData, WorksForEveryListImplementation) {
    std::mt19937 generator(1);
    VectorList<int> vectorList;
    ArrayList<int> arrayList;
    LinkedList<int> linkedList;
    fillRandom(vectorList, 5, generator);
    fillRandom(arrayList, 5, generator);
    fillRandom(linkedList, 5, generator);
    EXPECT_EQ(vectorList.size(), 5u);
    EXPECT_EQ(arrayList.size(), 5u);
    EXPECT_EQ(linkedList.size(), 5u);
}

TEST(RandomData, IntsAreInRange) {
    std::mt19937 generator(3);
    for (int i = 0; i < 200; ++i) {
        const int value = randomValue<int>(generator);
        EXPECT_GE(value, -100);
        EXPECT_LE(value, 100);
    }
}

TEST(RandomData, DoublesAreInRange) {
    std::mt19937 generator(4);
    for (int i = 0; i < 200; ++i) {
        const double value = randomValue<double>(generator);
        EXPECT_GE(value, -100.0);
        EXPECT_LE(value, 100.0);
    }
}

TEST(RandomData, StringsHaveValidLengthAndLetters) {
    std::mt19937 generator(5);
    for (int i = 0; i < 200; ++i) {
        const std::string value = randomValue<std::string>(generator);
        EXPECT_GE(value.size(), 3u);
        EXPECT_LE(value.size(), 8u);
        for (char letter : value) {
            EXPECT_TRUE(letter >= 'a' && letter <= 'z');
        }
    }
}

TEST(RandomData, VectorsHaveValidSize) {
    std::mt19937 generator(6);
    for (int i = 0; i < 200; ++i) {
        const std::vector<int> value = randomValue<std::vector<int>>(generator);
        EXPECT_LE(value.size(), 5u);
    }
}

TEST(RandomData, CommitsAreValid) {
    std::mt19937 generator(7);
    for (int i = 0; i < 100; ++i) {
        const Commit commit = randomValue<Commit>(generator);
        EXPECT_FALSE(commit.id().empty());
        EXPECT_EQ(commit.id()[0], 'c');
        EXPECT_FALSE(commit.author().empty());
        EXPECT_FALSE(commit.message().empty());
        EXPECT_GE(commit.linesChanged(), 1);
        EXPECT_LE(commit.linesChanged(), 500);
        EXPECT_GE(commit.timestamp(), 1700000000);
        EXPECT_LE(commit.timestamp(), 1760000000);
    }
}

// ---------- Сортування випадкових даних різних типів ----------

TEST(RandomSorting, Ints) {
    expectAllAlgorithmsSortRandomData<int>();
}

TEST(RandomSorting, Doubles) {
    expectAllAlgorithmsSortRandomData<double>();
}

TEST(RandomSorting, Strings) {
    expectAllAlgorithmsSortRandomData<std::string>();
}

TEST(RandomSorting, Vectors) {
    expectAllAlgorithmsSortRandomData<std::vector<int>>();
}

TEST(RandomSorting, Commits) {
    expectAllAlgorithmsSortRandomData<Commit>();
}