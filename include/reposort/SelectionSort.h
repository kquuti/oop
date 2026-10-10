#pragma once

#include <cstddef>
#include <string>

#include "reposort/ISortStrategy.h"

/// Сортування вибором. Нестабільне, O(n^2), мало обмінів.
template <typename T>
class SelectionSort : public ISortStrategy<T> {
public:
    void sort(IList<T>& list, const Comparator<T>& cmp) const override {
        const std::size_t n = list.size();
        for (std::size_t i = 0; i + 1 < n; ++i) {
            // Шукаємо, який елемент серед [i, n) має стояти першим.
            std::size_t best = i;
            for (std::size_t j = i + 1; j < n; ++j) {
                if (cmp(list.get(j), list.get(best))) {
                    best = j;
                }
            }
            if (best != i) {
                list.swap(i, best);
            }
        }
    }

    std::string name() const override {
        return "Selection sort";
    }
};