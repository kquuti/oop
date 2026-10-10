#include <gtest/gtest.h>

#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "reposort/ArrayList.h"
#include "reposort/SortRegistry.h"
#include "reposort/StandardSorts.h"
#include "reposort/domain/Commit.h"

namespace {

// Власний алгоритм, якого немає в бібліотеці. Він показує, що новий алгоритм
// підключається до реєстру без змін у існуючому коді.
template <typename T>
class BubbleSort : public ISortStrategy<T> {
public:
    void sort(IList<T>& list, const Comparator<T>& cmp) const override {
        const std::size_t n = list.size();
        for (std::size_t pass = 0; pass + 1 < n; ++pass) {
            for (std::size_t i = 0; i + 1 < n - pass; ++i) {
                if (cmp(list.get(i + 1), list.get(i))) {
                    list.swap(i, i + 1);
                }
            }
        }
    }

    std::string name() const override {
        return "Bubble sort";
    }
};

std::vector<int> toVector(const IList<int>& list) {
    std::vector<int> result;
    for (std::size_t i = 0; i < list.size(); ++i) {
        result.push_back(list.get(i));
    }
    return result;
}

}  // namespace

TEST(SortRegistry, NewRegistryIsEmpty) {
    SortRegistry<int> registry;
    EXPECT_EQ(registry.size(), 0u);
    EXPECT_FALSE(registry.has("quick"));
    EXPECT_TRUE(registry.keys().empty());
}

TEST(SortRegistry, UnknownKeyThrows) {
    SortRegistry<int> registry;
    EXPECT_THROW(registry.create("nope"), std::invalid_argument);
}

TEST(SortRegistry, RegistersAndCreatesCustomAlgorithm) {
    SortRegistry<int> registry;
    registry.registerAlgorithm("bubble", [] { return std::make_unique<BubbleSort<int>>(); });

    EXPECT_TRUE(registry.has("bubble"));
    EXPECT_EQ(registry.size(), 1u);

    std::unique_ptr<ISortStrategy<int>> sorter = registry.create("bubble");
    EXPECT_EQ(sorter->name(), "Bubble sort");

    ArrayList<int> list;
    list.add(3);
    list.add(1);
    list.add(2);
    sorter->sort(list, naturalOrder<int>());
    EXPECT_EQ(toVector(list), (std::vector<int>{1, 2, 3}));
}

TEST(SortRegistry, DuplicateKeyThrows) {
    SortRegistry<int> registry;
    registry.registerAlgorithm("bubble", [] { return std::make_unique<BubbleSort<int>>(); });
    EXPECT_THROW(
        registry.registerAlgorithm("bubble", [] { return std::make_unique<BubbleSort<int>>(); }),
        std::invalid_argument);
}

TEST(SortRegistry, EmptyFactoryThrows) {
    SortRegistry<int> registry;
    EXPECT_THROW(registry.registerAlgorithm("empty", SortRegistry<int>::Factory()),
                 std::invalid_argument);
    EXPECT_FALSE(registry.has("empty"));
}

TEST(SortRegistry, EachCreateReturnsNewObject) {
    SortRegistry<int> registry;
    registry.registerAlgorithm("bubble", [] { return std::make_unique<BubbleSort<int>>(); });
    auto first = registry.create("bubble");
    auto second = registry.create("bubble");
    EXPECT_NE(first.get(), second.get());
}

TEST(StandardSorts, RegistersSixAlgorithms) {
    SortRegistry<int> registry;
    registerStandardSorts(registry);
    EXPECT_EQ(registry.keys(),
              (std::vector<std::string>{"heap", "insertion", "merge", "quick", "selection", "shell"}));
}

TEST(StandardSorts, AlgorithmsHaveNames) {
    SortRegistry<int> registry;
    registerStandardSorts(registry);
    EXPECT_EQ(registry.create("insertion")->name(), "Insertion sort");
    EXPECT_EQ(registry.create("selection")->name(), "Selection sort");
    EXPECT_EQ(registry.create("shell")->name(), "Shell sort");
    EXPECT_EQ(registry.create("merge")->name(), "Merge sort");
    EXPECT_EQ(registry.create("quick")->name(), "Quick sort");
    EXPECT_EQ(registry.create("heap")->name(), "Heap sort");
}

TEST(StandardSorts, EveryAlgorithmSortsNumbers) {
    SortRegistry<int> registry;
    registerStandardSorts(registry);
    for (const std::string& key : registry.keys()) {
        std::unique_ptr<ISortStrategy<int>> sorter = registry.create(key);
        ArrayList<int> list;
        for (int value : {5, 3, 9, 1, 3, 7}) {
            list.add(value);
        }
        sorter->sort(list, naturalOrder<int>());
        EXPECT_EQ(toVector(list), (std::vector<int>{1, 3, 3, 5, 7, 9})) << "algorithm: " << key;
    }
}

TEST(StandardSorts, EveryAlgorithmWorksForCommits) {
    SortRegistry<Commit> registry;
    registerStandardSorts(registry);
    for (const std::string& key : registry.keys()) {
        std::unique_ptr<ISortStrategy<Commit>> sorter = registry.create(key);
        ArrayList<Commit> list;
        list.add(Commit("c1", "Olena", 100, "m", 50));
        list.add(Commit("c2", "Ivan", 50, "m", 200));
        list.add(Commit("c3", "Olena", 30, "m", 5));
        sorter->sort(list, comparingBy<Commit>(&Commit::linesChanged));
        EXPECT_EQ(list.get(0).id(), "c3") << "algorithm: " << key;
        EXPECT_EQ(list.get(1).id(), "c1") << "algorithm: " << key;
        EXPECT_EQ(list.get(2).id(), "c2") << "algorithm: " << key;
    }
}