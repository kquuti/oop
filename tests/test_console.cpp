#include <gtest/gtest.h>

#include <algorithm>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "reposort/ConsoleApp.h"
#include "reposort/domain/SampleRepository.h"

namespace {

using Ids = std::vector<std::string>;

/// Запускає меню з заданим «введенням користувача» й повертає весь вивід.
std::string runApp(const std::string& input, Repository& repo, unsigned int seed = 1) {
    std::istringstream in(input);
    std::ostringstream out;
    ConsoleApp app(in, out, repo, seed);
    app.run();
    return out.str();
}

bool contains(const std::string& text, const std::string& part) {
    return text.find(part) != std::string::npos;
}

/// Повертає id комітів c1..c7 у тому порядку, в якому вони надруковані після marker.
Ids idsInOrder(const std::string& text, const std::string& marker) {
    const std::size_t start = text.find(marker);
    if (start == std::string::npos) {
        return {};
    }
    std::vector<std::pair<std::size_t, std::string>> found;
    for (const char* id : {"c1", "c2", "c3", "c4", "c5", "c6", "c7"}) {
        const std::size_t position = text.find(std::string(id) + " |", start);
        if (position != std::string::npos) {
            found.emplace_back(position, id);
        }
    }
    std::sort(found.begin(), found.end());
    Ids result;
    for (const auto& entry : found) {
        result.push_back(entry.second);
    }
    return result;
}

}  // namespace

// ---------- Виконання команд ----------

TEST(ConsoleApp, ExitCommandEndsProgram) {
    Repository repo("demo");
    EXPECT_TRUE(contains(runApp("0\n", repo), "Bye!"));
}

TEST(ConsoleApp, EndOfInputEndsProgram) {
    Repository repo("demo");
    EXPECT_TRUE(contains(runApp("", repo), "Bye!"));
}

TEST(ConsoleApp, UnknownCommandIsReported) {
    Repository repo("demo");
    const std::string output = runApp("9\nabc\n0\n", repo);
    EXPECT_TRUE(contains(output, "Unknown command: 9"));
    EXPECT_TRUE(contains(output, "Unknown command: abc"));
}

TEST(ConsoleApp, HandleCommandReturnsFalseOnlyForExit) {
    Repository repo("demo");
    std::istringstream in("");
    std::ostringstream out;
    ConsoleApp app(in, out, repo);
    EXPECT_TRUE(app.handleCommand("1"));
    EXPECT_TRUE(app.handleCommand("zzz"));
    EXPECT_FALSE(app.handleCommand("0"));
    EXPECT_FALSE(app.handleCommand("  0  "));
}

TEST(ConsoleApp, SummaryShowsRepositoryData) {
    Repository repo("demo");
    fillSampleRepository(repo);
    const std::string output = runApp("1\n0\n", repo);
    EXPECT_TRUE(contains(output, "Authors: 3"));
    EXPECT_TRUE(contains(output, "Commits: 7"));
    EXPECT_TRUE(contains(output, "Branches: 3"));
    EXPECT_TRUE(contains(output, "feature/sorting (2 commits)"));
    EXPECT_TRUE(contains(output, "Pull requests: 2"));
}

TEST(ConsoleApp, ListsCommits) {
    Repository repo("demo");
    fillSampleRepository(repo);
    const std::string output = runApp("2\n0\n", repo);
    EXPECT_TRUE(contains(output, "1. c1 | Olena"));
    EXPECT_TRUE(contains(output, "7. c7 | Olena"));
}

TEST(ConsoleApp, ListOnEmptyRepository) {
    Repository repo("empty");
    EXPECT_TRUE(contains(runApp("2\n0\n", repo), "No commits."));
}

// ---------- Сортування ----------

TEST(ConsoleApp, SortByLinesDescending) {
    Repository repo("demo");
    fillSampleRepository(repo);
    // Merge sort (3), ArrayList (1), поле "lines" (3), за спаданням (2), більше критеріїв немає.
    const std::string output = runApp("3\n3\n1\n3\n2\nn\n0\n", repo);
    EXPECT_TRUE(contains(output, "Sorted 7 commits with Merge sort on ArrayList"));
    EXPECT_EQ(idsInOrder(output, "Sorted 7 commits"),
              (Ids{"c5", "c2", "c4", "c1", "c7", "c6", "c3"}));
}

TEST(ConsoleApp, SortByTimeDescending) {
    Repository repo("demo");
    fillSampleRepository(repo);
    const std::string output = runApp("3\n3\n1\n2\n2\nn\n0\n", repo);
    EXPECT_EQ(idsInOrder(output, "Sorted 7 commits"),
              (Ids{"c7", "c6", "c5", "c4", "c3", "c2", "c1"}));
}

TEST(ConsoleApp, SortOnLinkedListWithSelectionSort) {
    Repository repo("demo");
    fillSampleRepository(repo);
    const std::string output = runApp("3\n5\n2\n3\n2\nn\n0\n", repo);
    EXPECT_TRUE(contains(output, "Sorted 7 commits with Selection sort on LinkedList"));
    EXPECT_EQ(idsInOrder(output, "Sorted 7 commits"),
              (Ids{"c5", "c2", "c4", "c1", "c7", "c6", "c3"}));
}

TEST(ConsoleApp, SortByTwoCriteria) {
    Repository repo("demo");
    fillSampleRepository(repo);
    // Спершу автор за зростанням, потім кількість рядків за спаданням.
    const std::string output = runApp("3\n3\n1\n1\n1\ny\n3\n2\nn\n0\n", repo);
    EXPECT_EQ(idsInOrder(output, "Sorted 7 commits"),
              (Ids{"c5", "c2", "c4", "c1", "c7", "c3", "c6"}));
}

TEST(ConsoleApp, EveryAlgorithmAndListGiveTheSameOrder) {
    const Ids expected = {"c2", "c5", "c4", "c1", "c3", "c7", "c6"};  // автор, потім id
    for (int algorithm = 1; algorithm <= 6; ++algorithm) {
        for (int listType = 1; listType <= 3; ++listType) {
            Repository repo("demo");
            fillSampleRepository(repo);
            const std::string input = "3\n" + std::to_string(algorithm) + "\n" +
                                      std::to_string(listType) + "\n1\n1\nn\n0\n";
            const std::string output = runApp(input, repo);
            EXPECT_EQ(idsInOrder(output, "Sorted 7 commits"), expected)
                << "algorithm " << algorithm << ", list " << listType;
        }
    }
}

TEST(ConsoleApp, SortingDoesNotChangeRepositoryOrder) {
    Repository repo("demo");
    fillSampleRepository(repo);
    runApp("3\n3\n1\n3\n2\nn\n0\n", repo);
    EXPECT_EQ(repo.commits().get(0).id(), "c1");
    EXPECT_EQ(repo.commits().get(6).id(), "c7");
}

TEST(ConsoleApp, InvalidInputIsAskedAgain) {
    Repository repo("demo");
    fillSampleRepository(repo);
    // "abc" і "99" не підходять для вибору алгоритму; "maybe" не підходить для y/n.
    const std::string output = runApp("3\nabc\n99\n3\n1\n1\n1\nmaybe\nn\n0\n", repo);
    EXPECT_TRUE(contains(output, "Please enter a number from 1 to 6."));
    EXPECT_TRUE(contains(output, "Please answer y or n."));
    EXPECT_TRUE(contains(output, "Sorted 7 commits"));
}

TEST(ConsoleApp, InputEndingInTheMiddleOfSortingIsSafe) {
    Repository repo("demo");
    fillSampleRepository(repo);
    const std::string output = runApp("3\n3\n", repo);
    EXPECT_FALSE(contains(output, "Sorted"));
    EXPECT_TRUE(contains(output, "Bye!"));
}

TEST(ConsoleApp, SortOnEmptyRepository) {
    Repository repo("empty");
    EXPECT_TRUE(contains(runApp("3\n0\n", repo), "No commits to sort."));
}

// ---------- Випадкові коміти ----------

TEST(ConsoleApp, AddsRandomCommits) {
    Repository repo("demo");
    fillSampleRepository(repo);
    const std::string output = runApp("4\n10\n0\n", repo);
    EXPECT_TRUE(contains(output, "Added "));
    EXPECT_GT(repo.commitCount(), 7u);
    EXPECT_LE(repo.commitCount(), 17u);
}

TEST(ConsoleApp, RandomCommitsAreReproducibleWithSameSeed) {
    Repository first("a");
    Repository second("b");
    const std::string firstOutput = runApp("4\n20\n0\n", first, 42);
    const std::string secondOutput = runApp("4\n20\n0\n", second, 42);
    EXPECT_EQ(first.commitCount(), second.commitCount());
    EXPECT_EQ(firstOutput.substr(firstOutput.find("Added")),
              secondOutput.substr(secondOutput.find("Added")));
}

TEST(ConsoleApp, RandomCommitCountIsValidated) {
    Repository repo("demo");
    const std::string output = runApp("4\n0\n5000\n3\n0\n", repo);
    EXPECT_TRUE(contains(output, "Please enter a number from 1 to 1000."));
    EXPECT_GT(repo.commitCount(), 0u);
    EXPECT_LE(repo.commitCount(), 3u);
}

// ---------- Правила ----------

TEST(ConsoleApp, CheckRulesFindsAllViolations) {
    Repository repo("demo");
    fillSampleRepository(repo);
    const std::string output = runApp("5\n0\n", repo);
    EXPECT_TRUE(contains(output, "Checked 4 rules."));
    EXPECT_TRUE(contains(output, "Violations found: 4"));
    EXPECT_TRUE(contains(output, "[CommitMessageRule]"));
    EXPECT_TRUE(contains(output, "[CommitSizeRule]"));
    EXPECT_TRUE(contains(output, "[BranchNamingRule]"));
    EXPECT_TRUE(contains(output, "unknown author 'Petro'"));
}

TEST(ConsoleApp, CheckRulesOnEmptyRepository) {
    Repository repo("empty");
    EXPECT_TRUE(contains(runApp("5\n0\n", repo), "No rule violations found."));
}

// ---------- Звіти ----------

TEST(ConsoleApp, ShowsEachReport) {
    Repository repo("demo");
    fillSampleRepository(repo);
    EXPECT_TRUE(contains(runApp("6\n1\n0\n", repo), "Olena: 3 commits, 214 lines"));
    EXPECT_TRUE(contains(runApp("6\n2\n0\n", repo), "c5 (Ivan): 520 lines"));
    EXPECT_TRUE(contains(runApp("6\n3\n0\n", repo), "total 2 (open 1, merged 1, closed 0)"));
}

TEST(ConsoleApp, ReportChoiceIsValidated) {
    Repository repo("demo");
    fillSampleRepository(repo);
    const std::string output = runApp("6\n9\n1\n0\n", repo);
    EXPECT_TRUE(contains(output, "Please enter a number from 1 to 3."));
    EXPECT_TRUE(contains(output, "Olena: 3 commits"));
}

// ---------- Демонстрація на різних типах ----------

TEST(ConsoleApp, DemoWorksForEveryDataType) {
    Repository repo("demo");
    const std::string output = runApp("7\n3\n0\n", repo);
    for (const char* title : {"int:", "double:", "string:", "vector<int>:", "Commit:"}) {
        EXPECT_TRUE(contains(output, title)) << title;
    }
    EXPECT_TRUE(contains(output, "after (Merge sort)"));
}