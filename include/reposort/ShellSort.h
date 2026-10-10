#pragma once

#include <cstddef>
#include <string>

#include "reposort/ISortStrategy.h"

/// Сортування Шелла (кроки n/2, n/4, ..., 1). Нестабільне.
template <typename T>
class ShellSort : public ISortStrategy<T> {
public:
    void sort(IList<T>& list, const Comparator<T>& cmp) const override {
        const std::size_t n = list.size();
        for (std::size_t gap = n / 2; gap > 0; gap /= 2) {
            // Сортування вставками, але елементи порівнюються через крок gap.
            for (std::size_t i = gap; i < n; ++i) {
                T key = list.get(i);
                std::size_t j = i;
                while (j >= gap && cmp(key, list.get(j - gap))) {
                    list.set(j, list.get(j - gap));
                    j -= gap;
                }
                list.set(j, key);
            }
        }
    }

    std::string name() const override {
        return "Shell sort";
    }
};