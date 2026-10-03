#pragma once

#include <cstddef>
#include <stdexcept>

#include "reposort/IList.h"

/// Реалізація списку на основі динамічного масиву з автоматичним розширенням.
/// Тип T має мати конструктор без параметрів.
template <typename T>
class ArrayList : public IList<T> {
public:
    ArrayList() = default;

    // Копіювання заборонено, бо клас володіє динамічною пам'яттю.
    ArrayList(const ArrayList&) = delete;
    ArrayList& operator=(const ArrayList&) = delete;

    ~ArrayList() override {
        delete[] data_;
    }

    void add(const T& item) override {
        if (size_ == capacity_) {
            grow();
        }
        data_[size_] = item;
        ++size_;
    }

    const T& get(std::size_t index) const override {
        checkIndex(index);
        return data_[index];
    }

    void set(std::size_t index, const T& item) override {
        checkIndex(index);
        data_[index] = item;
    }

    void removeAt(std::size_t index) override {
        checkIndex(index);
        // Зсуваємо всі елементи праворуч від index на одну позицію вліво.
        for (std::size_t i = index; i + 1 < size_; ++i) {
            data_[i] = data_[i + 1];
        }
        --size_;
    }

    std::size_t size() const override {
        return size_;
    }

private:
    void checkIndex(std::size_t index) const {
        if (index >= size_) {
            throw std::out_of_range("ArrayList: index out of range");
        }
    }

    // Подвоює місткість: новий масив, копіювання, видалення старого.
    void grow() {
        std::size_t newCapacity = (capacity_ == 0) ? 4 : capacity_ * 2;
        T* newData = new T[newCapacity];
        for (std::size_t i = 0; i < size_; ++i) {
            newData[i] = data_[i];
        }
        delete[] data_;
        data_ = newData;
        capacity_ = newCapacity;
    }

    T* data_ = nullptr;
    std::size_t size_ = 0;
    std::size_t capacity_ = 0;
};