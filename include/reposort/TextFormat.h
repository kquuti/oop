#pragma once

#include <cstddef>
#include <sstream>
#include <string>
#include <vector>

// Попередні оголошення потрібні, щоб версії могли викликати одна одну
// (наприклад, vector<vector<int>>).
template <typename T>
std::string toText(const T& value);

template <typename T>
std::string toText(const std::vector<T>& values);

/// Текстове подання довільного значення, для якого визначено operator<<.
template <typename T>
std::string toText(const T& value) {
    std::ostringstream stream;
    stream << value;
    return stream.str();
}

/// Текстове подання вектора у вигляді [a, b, c].
template <typename T>
std::string toText(const std::vector<T>& values) {
    std::string result = "[";
    for (std::size_t i = 0; i < values.size(); ++i) {
        if (i > 0) {
            result += ", ";
        }
        result += toText(values[i]);
    }
    result += "]";
    return result;
}