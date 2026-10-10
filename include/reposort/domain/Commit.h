#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

/// Один коміт у репозиторії.
class Commit {
public:
    /// Порожній коміт. Потрібен, бо ArrayList створює масив через new T[n].
    Commit() = default;

    Commit(const std::string& id, const std::string& author,
           std::int64_t timestamp, const std::string& message, int linesChanged)
        : id_(id),
          author_(author),
          timestamp_(timestamp),
          message_(message),
          linesChanged_(linesChanged) {}

    const std::string& id() const { return id_; }
    const std::string& author() const { return author_; }
    std::int64_t timestamp() const { return timestamp_; }
    const std::string& message() const { return message_; }
    int linesChanged() const { return linesChanged_; }

    /// Чи вважається коміт «великим»: змінено не менше threshold рядків.
    bool isLarge(int threshold) const {
        return linesChanged_ >= threshold;
    }

    /// Повідомлення, обрізане до maxLength символів (з "..." наприкінці, якщо обрізано).
    std::string shortMessage(std::size_t maxLength) const {
        if (message_.size() <= maxLength) {
            return message_;
        }
        if (maxLength <= 3) {
            return message_.substr(0, maxLength);
        }
        return message_.substr(0, maxLength - 3) + "...";
    }

    /// Текстове подання коміту для виводу на екран.
    std::string toString() const {
        return id_ + " | " + author_ + " | " + std::to_string(timestamp_) + " | " +
               std::to_string(linesChanged_) + " lines | " + shortMessage(30);
    }

private:
    std::string id_;
    std::string author_;
    std::int64_t timestamp_ = 0;
    std::string message_;
    int linesChanged_ = 0;
};