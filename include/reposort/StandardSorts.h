#pragma once

#include <memory>

#include "reposort/HeapSort.h"
#include "reposort/InsertionSort.h"
#include "reposort/MergeSort.h"
#include "reposort/QuickSort.h"
#include "reposort/SelectionSort.h"
#include "reposort/ShellSort.h"
#include "reposort/SortRegistry.h"

/// Реєструє в реєстрі шість стандартних алгоритмів сортування.
template <typename T>
void registerStandardSorts(SortRegistry<T>& registry) {
    registry.registerAlgorithm("insertion", [] { return std::make_unique<InsertionSort<T>>(); });
    registry.registerAlgorithm("selection", [] { return std::make_unique<SelectionSort<T>>(); });
    registry.registerAlgorithm("shell", [] { return std::make_unique<ShellSort<T>>(); });
    registry.registerAlgorithm("merge", [] { return std::make_unique<MergeSort<T>>(); });
    registry.registerAlgorithm("quick", [] { return std::make_unique<QuickSort<T>>(); });
    registry.registerAlgorithm("heap", [] { return std::make_unique<HeapSort<T>>(); });
}