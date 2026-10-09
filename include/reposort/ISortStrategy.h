#pragma once

#include <string>

#include "reposort/Comparator.h"
#include "reposort/IList.h"

/// Інтерфейс алгоритму сортування списку.
template <typename T>
class ISortStrategy {
public:
    virtual ~ISortStrategy() = default;

    /// Сортує список на місці за заданим компаратором.
    virtual void sort(IList<T>& list, const Comparator<T>& cmp) const = 0;

    /// Назва алгоритму (для виводу користувачу).
    virtual std::string name() const = 0;
};