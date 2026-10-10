#pragma once

#include <cstddef>
#include <stdexcept>
#include <string>
#include <vector>

#include "reposort/ArrayList.h"
#include "reposort/domain/Author.h"
#include "reposort/domain/Branch.h"
#include "reposort/domain/Commit.h"
#include "reposort/domain/PullRequest.h"

/// Репозиторій: автори, коміти, гілки та pull requests.
/// Клас сам стежить за коректністю даних і кидає std::invalid_argument
/// при спробі додати дублікат або посилання на неіснуючий елемент.
/// Копіювання заборонене (через ArrayList усередині).
class Repository {
public:
    explicit Repository(const std::string& name) : name_(name) {}

    const std::string& name() const { return name_; }

    // ---------- Автори ----------

    void addAuthor(const Author& author) {
        if (findAuthor(author.name()) != nullptr) {
            throw std::invalid_argument("Repository: duplicate author: " + author.name());
        }
        authors_.push_back(author);
    }

    /// Повертає автора за ім'ям або nullptr, якщо такого немає.
    const Author* findAuthor(const std::string& name) const {
        for (const Author& author : authors_) {
            if (author.name() == name) {
                return &author;
            }
        }
        return nullptr;
    }

    const std::vector<Author>& authors() const { return authors_; }

    // ---------- Коміти ----------

    void addCommit(const Commit& commit) {
        if (findCommit(commit.id()) != nullptr) {
            throw std::invalid_argument("Repository: duplicate commit: " + commit.id());
        }
        commits_.add(commit);
    }

    /// Повертає коміт за id або nullptr, якщо такого немає.
    const Commit* findCommit(const std::string& id) const {
        for (std::size_t i = 0; i < commits_.size(); ++i) {
            if (commits_.get(i).id() == id) {
                return &commits_.get(i);
            }
        }
        return nullptr;
    }

    const IList<Commit>& commits() const { return commits_; }

    std::size_t commitCount() const { return commits_.size(); }

    /// Усі коміти заданого автора в порядку додавання.
    std::vector<Commit> commitsByAuthor(const std::string& authorName) const {
        std::vector<Commit> result;
        for (std::size_t i = 0; i < commits_.size(); ++i) {
            if (commits_.get(i).author() == authorName) {
                result.push_back(commits_.get(i));
            }
        }
        return result;
    }

    /// Додає копії всіх комітів у задану колекцію (будь-якої реалізації IList).
    /// Корисно, щоб відсортувати коміти, не змінюючи порядок в самому репозиторії.
    void copyCommitsTo(IList<Commit>& target) const {
        for (std::size_t i = 0; i < commits_.size(); ++i) {
            target.add(commits_.get(i));
        }
    }

    // ---------- Гілки ----------

    void addBranch(const Branch& branch) {
        if (findBranch(branch.name()) != nullptr) {
            throw std::invalid_argument("Repository: duplicate branch: " + branch.name());
        }
        branches_.push_back(branch);
    }

    const Branch* findBranch(const std::string& name) const {
        for (const Branch& branch : branches_) {
            if (branch.name() == name) {
                return &branch;
            }
        }
        return nullptr;
    }

    const std::vector<Branch>& branches() const { return branches_; }

    /// Додає існуючий коміт в існуючу гілку. Кидає std::invalid_argument,
    /// якщо невідома гілка або невідомий коміт.
    void addCommitToBranch(const std::string& branchName, const std::string& commitId) {
        Branch& branch = requireBranch(branchName);
        if (findCommit(commitId) == nullptr) {
            throw std::invalid_argument("Repository: unknown commit: " + commitId);
        }
        branch.addCommit(commitId);
    }

    // ---------- Pull requests ----------

    /// Додає pull request. Гілки source і target мають існувати, id має бути новим.
    void addPullRequest(const PullRequest& pullRequest) {
        if (findPullRequest(pullRequest.id()) != nullptr) {
            throw std::invalid_argument("Repository: duplicate pull request: " +
                                        std::to_string(pullRequest.id()));
        }
        requireBranch(pullRequest.sourceBranch());
        requireBranch(pullRequest.targetBranch());
        pullRequests_.push_back(pullRequest);
    }

    const PullRequest* findPullRequest(int id) const {
        for (const PullRequest& pullRequest : pullRequests_) {
            if (pullRequest.id() == id) {
                return &pullRequest;
            }
        }
        return nullptr;
    }

    const std::vector<PullRequest>& pullRequests() const { return pullRequests_; }

    /// Зливає pull request. Кидає std::invalid_argument, якщо id невідомий,
    /// і std::logic_error, якщо pull request уже не відкритий.
    void mergePullRequest(int id) {
        requirePullRequest(id).merge();
    }

    /// Закриває pull request (правила ті самі, що й для mergePullRequest).
    void closePullRequest(int id) {
        requirePullRequest(id).close();
    }

private:
    Branch& requireBranch(const std::string& name) {
        for (Branch& branch : branches_) {
            if (branch.name() == name) {
                return branch;
            }
        }
        throw std::invalid_argument("Repository: unknown branch: " + name);
    }

    PullRequest& requirePullRequest(int id) {
        for (PullRequest& pullRequest : pullRequests_) {
            if (pullRequest.id() == id) {
                return pullRequest;
            }
        }
        throw std::invalid_argument("Repository: unknown pull request: " + std::to_string(id));
    }

    std::string name_;
    std::vector<Author> authors_;
    ArrayList<Commit> commits_;
    std::vector<Branch> branches_;
    std::vector<PullRequest> pullRequests_;
};