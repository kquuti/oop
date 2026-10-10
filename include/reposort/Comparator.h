#pragma once

#include <functional>

/// Компаратор: повертає true, якщо a має стояти строго перед b.
template <typename T>
using Comparator = std::function<bool(const T&, const T&)>;

/// Природний порядок за зростанням (потрібен оператор < для типу T).
template <typename T>
Comparator<T> naturalOrder() {
    return [](const T& a, const T& b) { return a < b; };
}

/// Повертає компаратор з протилежним напрямком.
template <typename T>
Comparator<T> reversed(Comparator<T> cmp) {
    return [cmp](const T& a, const T& b) { return cmp(b, a); };
}

/// Компаратор за ключем: порівнює key(a) і key(b) оператором <.
/// key може бути лямбдою, функцією або вказівником на метод (наприклад &Commit::author).
template <typename T, typename KeyFunction>
Comparator<T> comparingBy(KeyFunction key) {
    return [key](const T& a, const T& b) {
        return std::invoke(key, a) < std::invoke(key, b);
    };
}