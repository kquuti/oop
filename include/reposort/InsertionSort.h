#pragma once

#include <cstddef>
#include <string>

#include "reposort/ISortStrategy.h"

/// Сортування вставками. Стабільне, O(n^2).
template <typename T>
class InsertionSort : public ISortStrategy<T> {
public:
    void sort(IList<T>& list, const Comparator<T>& cmp) const override {
        for (std::size_t i = 1; i < list.size(); ++i) {
            T key = list.get(i);
            std::size_t j = i;
            // Зсуваємо більші елементи вправо, поки не знайдемо місце для key.
            while (j > 0 && cmp(key, list.get(j - 1))) {
                list.set(j, list.get(j - 1));
                --j;
            }
            list.set(j, key);
        }
    }

    std::string name() const override {
        return "Insertion sort";
    }
};