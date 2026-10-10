#pragma once

#include <cstddef>
#include <random>
#include <string>
#include <vector>

#include "reposort/IList.h"

/// Генератор випадкового значення типу T. Загальний шаблон не визначений:
/// для кожного типу потрібна власна спеціалізація.
template <typename T>
struct RandomValue;

template <>
struct RandomValue<int> {
    static int make(std::mt19937& generator) {
        std::uniform_int_distribution<int> distribution(-100, 100);
        return distribution(generator);
    }
};

template <>
struct RandomValue<double> {
    static double make(std::mt19937& generator) {
        std::uniform_real_distribution<double> distribution(-100.0, 100.0);
        return distribution(generator);
    }
};

/// Рядок з 3-8 малих латинських літер.
template <>
struct RandomValue<std::string> {
    static std::string make(std::mt19937& generator) {
        std::uniform_int_distribution<int> length(3, 8);
        // Розподіл по int, а не по char: для char він не дозволений стандартом.
        std::uniform_int_distribution<int> letter('a', 'z');
        std::string result;
        const int size = length(generator);
        for (int i = 0; i < size; ++i) {
            result += static_cast<char>(letter(generator));
        }
        return result;
    }
};

/// Вектор довжиною 0-5 із випадкових елементів типу T.
template <typename T>
struct RandomValue<std::vector<T>> {
    static std::vector<T> make(std::mt19937& generator) {
        std::uniform_int_distribution<int> length(0, 5);
        std::vector<T> result;
        const int size = length(generator);
        for (int i = 0; i < size; ++i) {
            result.push_back(RandomValue<T>::make(generator));
        }
        return result;
    }
};

/// Повертає одне випадкове значення типу T.
template <typename T>
T randomValue(std::mt19937& generator) {
    return RandomValue<T>::make(generator);
}

/// Додає у список count випадкових елементів. Працює з будь-якою реалізацією IList.
template <typename T>
void fillRandom(IList<T>& list, std::size_t count, std::mt19937& generator) {
    for (std::size_t i = 0; i < count; ++i) {
        list.add(randomValue<T>(generator));
    }
}