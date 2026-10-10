#pragma once

#include <cstddef>
#include <string>

#include "reposort/ISortStrategy.h"

/// Піраміальне сортування. Нестабільне, O(n log n), без додаткової пам'яті.
template <typename T>
class HeapSort : public ISortStrategy<T> {
public:
    void sort(IList<T>& list, const Comparator<T>& cmp) const override {
        const std::size_t n = list.size();
        if (n < 2) {
            return;
        }
        // 1. Будуємо купу: просіюємо вниз усі батьківські вузли, з останнього до кореня.
        for (std::size_t start = n / 2; start > 0; --start) {
            siftDown(list, start - 1, n, cmp);
        }
        // 2. Корінь (елемент, що має бути останнім) ставимо в кінець і зменшуємо купу.
        for (std::size_t end = n; end > 1; --end) {
            list.swap(0, end - 1);
            siftDown(list, 0, end - 1, cmp);
        }
    }

    std::string name() const override {
        return "Heap sort";
    }

private:
    // Просіює елемент root вниз по купі, що займає індекси [0, end).
    static void siftDown(IList<T>& list, std::size_t root, std::size_t end,
                         const Comparator<T>& cmp) {
        while (2 * root + 1 < end) {
            std::size_t child = 2 * root + 1;  // ліва дитина
            // Обираємо ту дитину, що має стояти пізніше.
            if (child + 1 < end && cmp(list.get(child), list.get(child + 1))) {
                ++child;
            }
            // Якщо батько вже не стоїть перед дитиною, купа правильна.
            if (!cmp(list.get(root), list.get(child))) {
                return;
            }
            list.swap(root, child);
            root = child;
        }
    }
};