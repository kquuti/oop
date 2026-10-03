#pragma once

#include <cstddef>

/// Інтерфейс списку елементів типу T.
template <typename T>
class IList {
public:
    virtual ~IList() = default;

    /// Додає елемент в кінець списку.
    virtual void add(const T& item) = 0;

    /// Повертає елемент за індексом. Кидає std::out_of_range, якщо індекс неправильний.
    virtual const T& get(std::size_t index) const = 0;

    /// Замінює елемент за індексом. Кидає std::out_of_range, якщо індекс неправильний.
    virtual void set(std::size_t index, const T& item) = 0;

    /// Видаляє елемент за індексом. Кидає std::out_of_range, якщо індекс неправильний.
    virtual void removeAt(std::size_t index) = 0;

    /// Кількість елементів.
    virtual std::size_t size() const = 0;

    /// Чи порожній список.
    bool isEmpty() const {
        return size() == 0;
    }

    /// Міняє місцями два елементи (потрібно буде для сортування).
    void swap(std::size_t i, std::size_t j) {
        T temp = get(i);
        set(i, get(j));
        set(j, temp);
    }
};