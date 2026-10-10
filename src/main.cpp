#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "reposort/ArrayList.h"
#include "reposort/RandomData.h"
#include "reposort/SortRegistry.h"
#include "reposort/StandardSorts.h"
#include "reposort/domain/Commit.h"
#include "reposort/domain/RandomCommit.h"

namespace {

// Створює список випадкових елементів типу T, друкує його до і після сортування.
template <typename T>
void demo(const std::string& title, std::mt19937& generator) {
    ArrayList<T> list;
    fillRandom(list, 4, generator);
    std::cout << "=== " << title << " ===\n";
    std::cout << "Before: " << list.toString() << "\n";

    SortRegistry<T> registry;
    registerStandardSorts(registry);
    const auto sorter = registry.create("merge");
    sorter->sort(list, naturalOrder<T>());
    std::cout << "After (" << sorter->name() << "): " << list.toString() << "\n\n";
}

}  // namespace

int main() {
    std::mt19937 generator(2025);
    demo<int>("int", generator);
    demo<double>("double", generator);
    demo<std::string>("string", generator);
    demo<std::vector<int>>("vector<int>", generator);
    demo<Commit>("Commit", generator);
    return 0;
}