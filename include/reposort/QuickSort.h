#pragma once

#include <cstddef>
#include <string>

#include "reposort/ISortStrategy.h"

/// Швидке сортування (схема Ломуто, опорний елемент із середини).
/// Нестабільне, у середньому O(n log n).
template <typename T>
class QuickSort : public ISortStrategy<T> {
public:
    void sort(IList<T>& list, const Comparator<T>& cmp) const override {
        if (list.size() < 2) {
            return;
        }
        quickSort(list, 0, static_cast<Index>(list.size()) - 1, cmp);
    }

    std::string name() const override {
        return "Quick sort";
    }

private:
    // Знаковий тип, бо в рекурсії border - 1 може стати -1.
    using Index = std::ptrdiff_t;

    static void swapItems(IList<T>& list, Index i, Index j) {
        list.swap(static_cast<std::size_t>(i), static_cast<std::size_t>(j));
    }

    // Сортує діапазон [low, high] (обидві межі включно).
    static void quickSort(IList<T>& list, Index low, Index high,
                          const Comparator<T>& cmp) {
        if (low >= high) {
            return;
        }
        // Опорний елемент із середини переносимо в кінець діапазону.
        const Index middle = low + (high - low) / 2;
        swapItems(list, middle, high);
        const T pivot = list.get(static_cast<std::size_t>(high));

        // Усе зліва від border строго менше за pivot.
        Index border = low;
        for (Index i = low; i < high; ++i) {
            if (cmp(list.get(static_cast<std::size_t>(i)), pivot)) {
                swapItems(list, i, border);
                ++border;
            }
        }
        swapItems(list, border, high);  // pivot стає на своє остаточне місце

        quickSort(list, low, border - 1, cmp);
        quickSort(list, border + 1, high, cmp);
    }
};