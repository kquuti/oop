#pragma once

#include <cstddef>
#include <stdexcept>
#include <vector>

#include "reposort/IList.h"

/// Реалізація списку на основі std::vector.
template <typename T>
class VectorList : public IList<T> {
public:
    void add(const T& item) override {
        data_.push_back(item);
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
        data_.erase(data_.begin() + static_cast<std::ptrdiff_t>(index));
    }

    std::size_t size() const override {
        return data_.size();
    }

private:
    void checkIndex(std::size_t index) const {
        if (index >= data_.size()) {
            throw std::out_of_range("VectorList: index out of range");
        }
    }

    std::vector<T> data_;
};