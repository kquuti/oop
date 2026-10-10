#pragma once

#include <cstddef>
#include <vector>

#include "reposort/Comparator.h"

/// Ланцюжок компараторів: порівнює за першим критерієм, а за однакових значень
/// переходить до наступного і так далі.
template <typename T>
class ComparatorChain {
public:
    /// Додає наступний критерій. Повертає сам ланцюжок для запису chain.add(x).add(y).
    ComparatorChain& add(const Comparator<T>& cmp) {
        comparators_.push_back(cmp);
        return *this;
    }

    /// Кількість критеріїв.
    std::size_t size() const {
        return comparators_.size();
    }

    /// Чи має a стояти строго перед b. Дозволяє викликати ланцюжок як функцію.
    bool operator()(const T& a, const T& b) const {
        for (const auto& cmp : comparators_) {
            if (cmp(a, b)) {
                return true;
            }
            if (cmp(b, a)) {
                return false;
            }
            // За цим критерієм елементи рівні, переходимо до наступного.
        }
        return false;  // рівні за всіма критеріями
    }

private:
    std::vector<Comparator<T>> comparators_;
};