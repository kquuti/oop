#pragma once

#include <cstddef>
#include <string>

/// Автор комітів: ім'я та електронна пошта.
class Author {
public:
    Author() = default;

    Author(const std::string& name, const std::string& email)
        : name_(name), email_(email) {}

    const std::string& name() const { return name_; }
    const std::string& email() const { return email_; }

    /// Проста перевірка формату пошти: рівно один '@', непорожня частина перед ним,
    /// а після нього домен із крапкою всередині (наприклад a@b.c).
    bool isValidEmail() const {
        const std::size_t at = email_.find('@');
        if (at == std::string::npos || at == 0) {
            return false;
        }
        if (email_.find('@', at + 1) != std::string::npos) {
            return false;
        }
        const std::size_t dot = email_.find('.', at + 1);
        return dot != std::string::npos && dot > at + 1 && dot + 1 < email_.size();
    }

    /// Домен пошти (частина після '@') або порожній рядок, якщо '@' немає.
    std::string emailDomain() const {
        const std::size_t at = email_.find('@');
        if (at == std::string::npos) {
            return "";
        }
        return email_.substr(at + 1);
    }

private:
    std::string name_;
    std::string email_;
};