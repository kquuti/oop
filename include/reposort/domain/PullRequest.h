#pragma once

#include <stdexcept>
#include <string>

/// Стан pull request.
enum class PullRequestState { Open, Merged, Closed };

/// Pull request: пропозиція злити одну гілку в іншу.
class PullRequest {
public:
    PullRequest(int id, const std::string& title, const std::string& author,
                const std::string& sourceBranch, const std::string& targetBranch)
        : id_(id),
          title_(title),
          author_(author),
          sourceBranch_(sourceBranch),
          targetBranch_(targetBranch) {}

    int id() const { return id_; }
    const std::string& title() const { return title_; }
    const std::string& author() const { return author_; }
    const std::string& sourceBranch() const { return sourceBranch_; }
    const std::string& targetBranch() const { return targetBranch_; }
    PullRequestState state() const { return state_; }

    bool isOpen() const {
        return state_ == PullRequestState::Open;
    }

    /// Зливає pull request. Кидає std::logic_error, якщо він уже не відкритий.
    void merge() {
        requireOpen("merge");
        state_ = PullRequestState::Merged;
    }

    /// Закриває pull request без злиття. Кидає std::logic_error, якщо він уже не відкритий.
    void close() {
        requireOpen("close");
        state_ = PullRequestState::Closed;
    }

private:
    void requireOpen(const std::string& action) const {
        if (!isOpen()) {
            throw std::logic_error("PullRequest " + std::to_string(id_) +
                                   ": cannot " + action + ", it is not open");
        }
    }

    int id_;
    std::string title_;
    std::string author_;
    std::string sourceBranch_;
    std::string targetBranch_;
    PullRequestState state_ = PullRequestState::Open;
};