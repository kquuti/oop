#pragma once

#include <cstddef>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "reposort/domain/Repository.h"

/// Одне порушення правила.
struct Violation {
    std::string rule;     ///< назва правила
    std::string message;  ///< опис порушення
};

/// Інтерфейс правила: перевіряє репозиторій і повертає знайдені порушення.
class IRule {
public:
    virtual ~IRule() = default;

    virtual std::string name() const = 0;
    virtual std::vector<Violation> check(const Repository& repo) const = 0;
};

/// Повідомлення коміту не коротше за minLength символів.
class CommitMessageRule : public IRule {
public:
    explicit CommitMessageRule(std::size_t minLength) : minLength_(minLength) {}

    std::string name() const override { return "CommitMessageRule"; }

    std::vector<Violation> check(const Repository& repo) const override {
        std::vector<Violation> result;
        const IList<Commit>& commits = repo.commits();
        for (std::size_t i = 0; i < commits.size(); ++i) {
            const Commit& commit = commits.get(i);
            if (commit.message().size() < minLength_) {
                result.push_back({name(), "Commit " + commit.id() + ": message is too short (" +
                                              std::to_string(commit.message().size()) + " < " +
                                              std::to_string(minLength_) + ")"});
            }
        }
        return result;
    }

private:
    std::size_t minLength_;
};

/// Коміт не змінює більше за maxLines рядків.
class CommitSizeRule : public IRule {
public:
    explicit CommitSizeRule(int maxLines) : maxLines_(maxLines) {}

    std::string name() const override { return "CommitSizeRule"; }

    std::vector<Violation> check(const Repository& repo) const override {
        std::vector<Violation> result;
        const IList<Commit>& commits = repo.commits();
        for (std::size_t i = 0; i < commits.size(); ++i) {
            const Commit& commit = commits.get(i);
            if (commit.linesChanged() > maxLines_) {
                result.push_back({name(), "Commit " + commit.id() + ": too many changed lines (" +
                                              std::to_string(commit.linesChanged()) + " > " +
                                              std::to_string(maxLines_) + ")"});
            }
        }
        return result;
    }

private:
    int maxLines_;
};

/// Назви гілок відповідають прийнятому стилю (див. Branch::hasConventionalName).
class BranchNamingRule : public IRule {
public:
    std::string name() const override { return "BranchNamingRule"; }

    std::vector<Violation> check(const Repository& repo) const override {
        std::vector<Violation> result;
        for (const Branch& branch : repo.branches()) {
            if (!branch.hasConventionalName()) {
                result.push_back({name(), "Branch '" + branch.name() + "': unconventional name"});
            }
        }
        return result;
    }
};

/// Автор кожного коміту зареєстрований у репозиторії.
class KnownAuthorRule : public IRule {
public:
    std::string name() const override { return "KnownAuthorRule"; }

    std::vector<Violation> check(const Repository& repo) const override {
        std::vector<Violation> result;
        const IList<Commit>& commits = repo.commits();
        for (std::size_t i = 0; i < commits.size(); ++i) {
            const Commit& commit = commits.get(i);
            if (repo.findAuthor(commit.author()) == nullptr) {
                result.push_back({name(), "Commit " + commit.id() + ": unknown author '" +
                                              commit.author() + "'"});
            }
        }
        return result;
    }
};

/// Набір правил, які перевіряються разом.
class RuleChecker {
public:
    /// Додає правило. Кидає std::invalid_argument для порожнього вказівника.
    void add(std::unique_ptr<IRule> rule) {
        if (!rule) {
            throw std::invalid_argument("RuleChecker: null rule");
        }
        rules_.push_back(std::move(rule));
    }

    std::size_t size() const { return rules_.size(); }

    /// Перевіряє репозиторій усіма правилами; порушення йдуть у порядку правил.
    std::vector<Violation> checkAll(const Repository& repo) const {
        std::vector<Violation> result;
        for (const auto& rule : rules_) {
            std::vector<Violation> found = rule->check(repo);
            result.insert(result.end(), found.begin(), found.end());
        }
        return result;
    }

private:
    std::vector<std::unique_ptr<IRule>> rules_;
};