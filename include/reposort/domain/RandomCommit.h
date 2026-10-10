#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <random>
#include <string>

#include "reposort/RandomData.h"
#include "reposort/domain/Commit.h"

/// Генерація випадкового коміту.
template <>
struct RandomValue<Commit> {
    static Commit make(std::mt19937& generator) {
        static const std::array<const char*, 5> authors = {"Olena", "Ivan", "Maria", "Petro", "Sofia"};
        static const std::array<const char*, 6> messages = {
            "Fix bug", "Add feature", "Refactor code", "Update docs", "Add tests", "Improve performance"};

        std::uniform_int_distribution<std::size_t> authorIndex(0, authors.size() - 1);
        std::uniform_int_distribution<std::size_t> messageIndex(0, messages.size() - 1);
        std::uniform_int_distribution<int> idNumber(1000, 9999);
        std::uniform_int_distribution<std::int64_t> time(1700000000, 1760000000);
        std::uniform_int_distribution<int> lines(1, 500);

        return Commit("c" + std::to_string(idNumber(generator)),
                      authors[authorIndex(generator)],
                      time(generator),
                      messages[messageIndex(generator)],
                      lines(generator));
    }
};