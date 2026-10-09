#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "reposort/ISortStrategy.h"

/// Сортування злиттям. Стабільне, O(n log n), потребує додаткової пам'яті.
template <typename T>
class MergeSort : public ISortStrategy<T> {
public:
    void sort(IList<T>& list, const Comparator<T>& cmp) const override {
        const std::size_t n = list.size();
        if (n < 2) {
            return;
        }
        // Копіюємо елементи у вектор, сортуємо його, пишемо результат назад.
        std::vector<T> items;
        items.reserve(n);
        for (std::size_t i = 0; i < n; ++i) {
            items.push_back(list.get(i));
        }
        std::vector<T> buffer(items);
        mergeSort(items, buffer, 0, n, cmp);
        for (std::size_t i = 0; i < n; ++i) {
            list.set(i, items[i]);
        }
    }

    std::string name() const override {
        return "Merge sort";
    }

private:
    // Сортує півінтервал [low, high).
    static void mergeSort(std::vector<T>& items, std::vector<T>& buffer,
                          std::size_t low, std::size_t high,
                          const Comparator<T>& cmp) {
        if (high - low < 2) {
            return;
        }
        const std::size_t middle = low + (high - low) / 2;
        mergeSort(items, buffer, low, middle, cmp);
        mergeSort(items, buffer, middle, high, cmp);
        merge(items, buffer, low, middle, high, cmp);
    }

    // Зливає дві відсортовані частини [low, middle) і [middle, high).
    static void merge(std::vector<T>& items, std::vector<T>& buffer,
                      std::size_t low, std::size_t middle, std::size_t high,
                      const Comparator<T>& cmp) {
        std::size_t left = low;
        std::size_t right = middle;
        std::size_t out = low;
        while (left < middle && right < high) {
            // Беремо з правої частини лише якщо вона строго менша: це дає стабільність.
            if (cmp(items[right], items[left])) {
                buffer[out++] = items[right++];
            } else {
                buffer[out++] = items[left++];
            }
        }
        while (left < middle) {
            buffer[out++] = items[left++];
        }
        while (right < high) {
            buffer[out++] = items[right++];
        }
        for (std::size_t i = low; i < high; ++i) {
            items[i] = buffer[i];
        }
    }
};