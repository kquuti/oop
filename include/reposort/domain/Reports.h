#pragma once

#include <algorithm>
#include <cstddef>
#include <map>
#include <string>

#include "reposort/ArrayList.h"
#include "reposort/ComparatorChain.h"
#include "reposort/MergeSort.h"
#include "reposort/domain/Repository.h"

/// Інтерфейс звіту: будує текстовий звіт про репозиторій.
class IReport {
public:
    virtual ~IReport() = default;

    virtual std::string name() const = 0;
    virtual std::string generate(const Repository& repo) const = 0;
};

/// Підсумок по одному автору.
struct AuthorStatistics {
    std::string author;
    std::size_t commitCount = 0;
    int totalLines = 0;
};

/// Кількість комітів і змінених рядків по авторах.
/// Спершу автори з більшою кількістю комітів, за однакової кількості за абеткою.
class CommitsPerAuthorReport : public IReport {
public:
    std::string name() const override { return "Commits per author"; }

    std::string generate(const Repository& repo) const override {
        std::map<std::string, AuthorStatistics> stats;
        const IList<Commit>& commits = repo.commits();
        for (std::size_t i = 0; i < commits.size(); ++i) {
            const Commit& commit = commits.get(i);
            AuthorStatistics& entry = stats[commit.author()];
            entry.author = commit.author();
            ++entry.commitCount;
            entry.totalLines += commit.linesChanged();
        }

        ArrayList<AuthorStatistics> list;
        for (const auto& pair : stats) {
            list.add(pair.second);
        }

        ComparatorChain<AuthorStatistics> chain;
        chain.add(reversed(comparingBy<AuthorStatistics>(
                      [](const AuthorStatistics& s) { return s.commitCount; })))
             .add(comparingBy<AuthorStatistics>(
                      [](const AuthorStatistics& s) { return s.author; }));
        MergeSort<AuthorStatistics>().sort(list, chain);

        std::string result = name() + "\n";
        if (list.isEmpty()) {
            return result + "  (no commits)\n";
        }
        for (std::size_t i = 0; i < list.size(); ++i) {
            const AuthorStatistics& s = list.get(i);
            result += "  " + s.author + ": " + std::to_string(s.commitCount) + " commits, " +
                      std::to_string(s.totalLines) + " lines\n";
        }
        return result;
    }
};

/// Найбільші коміти за кількістю змінених рядків (не більше topCount штук).
class LargestCommitsReport : public IReport {
public:
    explicit LargestCommitsReport(std::size_t topCount) : topCount_(topCount) {}

    std::string name() const override { return "Largest commits"; }

    std::string generate(const Repository& repo) const override {
        ArrayList<Commit> list;
        repo.copyCommitsTo(list);

        // Якщо розмір однаковий, порядок визначає id, тож результат завжди той самий.
        ComparatorChain<Commit> chain;
        chain.add(reversed(comparingBy<Commit>(&Commit::linesChanged)))
             .add(comparingBy<Commit>(&Commit::id));
        MergeSort<Commit>().sort(list, chain);

        std::string result = name() + "\n";
        if (list.isEmpty()) {
            return result + "  (no commits)\n";
        }
        const std::size_t shown = std::min(topCount_, list.size());
        for (std::size_t i = 0; i < shown; ++i) {
            const Commit& commit = list.get(i);
            result += "  " + commit.id() + " (" + commit.author() + "): " +
                      std::to_string(commit.linesChanged()) + " lines, " +
                      commit.shortMessage(30) + "\n";
        }
        return result;
    }

private:
    std::size_t topCount_;
};

/// Кількість pull requests за станами та перелік відкритих.
class PullRequestSummaryReport : public IReport {
public:
    std::string name() const override { return "Pull request summary"; }

    std::string generate(const Repository& repo) const override {
        std::size_t open = 0;
        std::size_t merged = 0;
        std::size_t closed = 0;
        std::string openList;
        for (const PullRequest& pullRequest : repo.pullRequests()) {
            switch (pullRequest.state()) {
                case PullRequestState::Open:
                    ++open;
                    openList += "  #" + std::to_string(pullRequest.id()) + " " +
                                pullRequest.title() + " (" + pullRequest.sourceBranch() +
                                " -> " + pullRequest.targetBranch() + ")\n";
                    break;
                case PullRequestState::Merged:
                    ++merged;
                    break;
                case PullRequestState::Closed:
                    ++closed;
                    break;
            }
        }

        std::string result = name() + "\n";
        result += "  total " + std::to_string(repo.pullRequests().size()) + " (open " +
                  std::to_string(open) + ", merged " + std::to_string(merged) + ", closed " +
                  std::to_string(closed) + ")\n";
        if (open > 0) {
            result += "  Open:\n" + openList;
        }
        return result;
    }
};