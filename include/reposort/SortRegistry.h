#pragma once

#include <cstddef>
#include <functional>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "reposort/ISortStrategy.h"

/// Реєстр алгоритмів сортування: зіставляє текстовий ключ і спосіб створення алгоритму.
/// Дозволяє підключати нові алгоритми без зміни існуючого коду.
template <typename T>
class SortRegistry {
public:
    /// Фабрика створює новий екземпляр алгоритму.
    using Factory = std::function<std::unique_ptr<ISortStrategy<T>>()>;

    /// Реєструє алгоритм. Кидає std::invalid_argument, якщо ключ уже зайнятий
    /// або фабрика порожня.
    void registerAlgorithm(const std::string& key, Factory factory) {
        if (!factory) {
            throw std::invalid_argument("SortRegistry: empty factory for key: " + key);
        }
        if (has(key)) {
            throw std::invalid_argument("SortRegistry: key already registered: " + key);
        }
        factories_[key] = std::move(factory);
    }

    /// Чи зареєстровано алгоритм з таким ключем.
    bool has(const std::string& key) const {
        return factories_.count(key) > 0;
    }

    /// Створює алгоритм за ключем. Кидає std::invalid_argument, якщо ключа немає.
    std::unique_ptr<ISortStrategy<T>> create(const std::string& key) const {
        const auto it = factories_.find(key);
        if (it == factories_.end()) {
            throw std::invalid_argument("SortRegistry: unknown algorithm: " + key);
        }
        return it->second();
    }

    /// Усі ключі за абеткою.
    std::vector<std::string> keys() const {
        std::vector<std::string> result;
        for (const auto& entry : factories_) {
            result.push_back(entry.first);
        }
        return result;
    }

    /// Кількість зареєстрованих алгоритмів.
    std::size_t size() const {
        return factories_.size();
    }

private:
    std::map<std::string, Factory> factories_;
};