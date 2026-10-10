#pragma once

#include <algorithm>
#include <cstddef>
#include <string>
#include <vector>

/// Гілка репозиторію: назва та впорядкований список ідентифікаторів комітів.
class Branch {
public:
    Branch() = default;

    explicit Branch(const std::string& name) : name_(name) {}

    const std::string& name() const { return name_; }
    const std::vector<std::string>& commitIds() const { return commitIds_; }

    std::size_t commitCount() const { return commitIds_.size(); }

    /// Додає коміт у гілку. Повторне додавання того самого id ігнорується.
    void addCommit(const std::string& commitId) {
        if (!containsCommit(commitId)) {
            commitIds_.push_back(commitId);
        }
    }

    bool containsCommit(const std::string& commitId) const {
        return std::find(commitIds_.begin(), commitIds_.end(), commitId) != commitIds_.end();
    }

    /// Чи є це головна гілка (main або master).
    bool isMain() const {
        return name_ == "main" || name_ == "master";
    }

    /// Чи відповідає назва прийнятому стилю: main, master, develop або
    /// префікс feature/, bugfix/, hotfix/, release/ з непорожнім продовженням.
    bool hasConventionalName() const {
        if (isMain() || name_ == "develop") {
            return true;
        }
        return startsWith("feature/") || startsWith("bugfix/") ||
               startsWith("hotfix/") || startsWith("release/");
    }

private:
    // Префікс має бути повністю присутній і після нього має щось йти.
    bool startsWith(const std::string& prefix) const {
        return name_.size() > prefix.size() && name_.compare(0, prefix.size(), prefix) == 0;
    }

    std::string name_;
    std::vector<std::string> commitIds_;
};