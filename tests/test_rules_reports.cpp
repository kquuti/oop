#include <gtest/gtest.h>

#include <memory>
#include <string>
#include <vector>

#include "reposort/domain/Reports.h"
#include "reposort/domain/Rules.h"
#include "reposort/domain/SampleRepository.h"

namespace {

bool contains(const std::string& text, const std::string& part) {
    return text.find(part) != std::string::npos;
}

}  // namespace

// ---------- Правила окремо ----------

TEST(Rules, CommitMessageRuleFindsShortMessages) {
    Repository repo("demo");
    fillSampleRepository(repo);
    const std::vector<Violation> found = CommitMessageRule(10).check(repo);
    ASSERT_EQ(found.size(), 1u);
    EXPECT_EQ(found[0].rule, "CommitMessageRule");
    EXPECT_TRUE(contains(found[0].message, "c3"));
}

TEST(Rules, CommitMessageRuleWithZeroLimitFindsNothing) {
    Repository repo("demo");
    fillSampleRepository(repo);
    EXPECT_TRUE(CommitMessageRule(0).check(repo).empty());
}

TEST(Rules, CommitSizeRuleFindsLargeCommits) {
    Repository repo("demo");
    fillSampleRepository(repo);
    const std::vector<Violation> found = CommitSizeRule(500).check(repo);
    ASSERT_EQ(found.size(), 1u);
    EXPECT_TRUE(contains(found[0].message, "c5"));
    EXPECT_TRUE(CommitSizeRule(520).check(repo).empty());  // межа дозволена
    EXPECT_EQ(CommitSizeRule(100).check(repo).size(), 4u);
}

TEST(Rules, BranchNamingRuleFindsBadNames) {
    Repository repo("demo");
    fillSampleRepository(repo);
    const std::vector<Violation> found = BranchNamingRule().check(repo);
    ASSERT_EQ(found.size(), 1u);
    EXPECT_TRUE(contains(found[0].message, "fix-typo"));
}

TEST(Rules, KnownAuthorRuleFindsUnknownAuthors) {
    Repository repo("demo");
    fillSampleRepository(repo);
    const std::vector<Violation> found = KnownAuthorRule().check(repo);
    ASSERT_EQ(found.size(), 1u);
    EXPECT_TRUE(contains(found[0].message, "Petro"));
}

TEST(Rules, EmptyRepositoryHasNoViolations) {
    Repository repo("empty");
    EXPECT_TRUE(CommitMessageRule(10).check(repo).empty());
    EXPECT_TRUE(CommitSizeRule(500).check(repo).empty());
    EXPECT_TRUE(BranchNamingRule().check(repo).empty());
    EXPECT_TRUE(KnownAuthorRule().check(repo).empty());
}

// ---------- RuleChecker і поліморфізм ----------

TEST(RuleChecker, CollectsViolationsFromAllRules) {
    Repository repo("demo");
    fillSampleRepository(repo);

    RuleChecker checker;
    checker.add(std::make_unique<CommitMessageRule>(10));
    checker.add(std::make_unique<CommitSizeRule>(500));
    checker.add(std::make_unique<BranchNamingRule>());
    checker.add(std::make_unique<KnownAuthorRule>());
    EXPECT_EQ(checker.size(), 4u);

    const std::vector<Violation> found = checker.checkAll(repo);
    ASSERT_EQ(found.size(), 4u);
    // Порядок порушень відповідає порядку додавання правил.
    EXPECT_EQ(found[0].rule, "CommitMessageRule");
    EXPECT_EQ(found[1].rule, "CommitSizeRule");
    EXPECT_EQ(found[2].rule, "BranchNamingRule");
    EXPECT_EQ(found[3].rule, "KnownAuthorRule");
}

TEST(RuleChecker, EmptyCheckerFindsNothing) {
    Repository repo("demo");
    fillSampleRepository(repo);
    EXPECT_TRUE(RuleChecker().checkAll(repo).empty());
}

TEST(RuleChecker, RejectsNullRule) {
    RuleChecker checker;
    EXPECT_THROW(checker.add(nullptr), std::invalid_argument);
    EXPECT_EQ(checker.size(), 0u);
}

TEST(Rules, WorkThroughCommonInterface) {
    Repository repo("demo");
    fillSampleRepository(repo);

    std::vector<std::unique_ptr<IRule>> rules;
    rules.push_back(std::make_unique<CommitMessageRule>(10));
    rules.push_back(std::make_unique<BranchNamingRule>());

    std::size_t total = 0;
    for (const auto& rule : rules) {
        EXPECT_FALSE(rule->name().empty());
        total += rule->check(repo).size();
    }
    EXPECT_EQ(total, 2u);
}

// ---------- Звіти ----------

TEST(Reports, CommitsPerAuthorIsSortedByCountThenName) {
    Repository repo("demo");
    fillSampleRepository(repo);
    const std::string report = CommitsPerAuthorReport().generate(repo);

    EXPECT_TRUE(contains(report, "Olena: 3 commits, 214 lines"));
    EXPECT_TRUE(contains(report, "Ivan: 2 commits, 860 lines"));
    EXPECT_TRUE(contains(report, "Maria: 1 commits, 210 lines"));
    EXPECT_TRUE(contains(report, "Petro: 1 commits, 35 lines"));

    // Порядок: Olena (3), Ivan (2), Maria (1), Petro (1).
    EXPECT_LT(report.find("Olena"), report.find("Ivan"));
    EXPECT_LT(report.find("Ivan"), report.find("Maria"));
    EXPECT_LT(report.find("Maria"), report.find("Petro"));
}

TEST(Reports, CommitsPerAuthorOnEmptyRepository) {
    Repository repo("empty");
    EXPECT_TRUE(contains(CommitsPerAuthorReport().generate(repo), "(no commits)"));
}

TEST(Reports, LargestCommitsShowsTopOnly) {
    Repository repo("demo");
    fillSampleRepository(repo);
    const std::string report = LargestCommitsReport(3).generate(repo);

    EXPECT_TRUE(contains(report, "c5"));
    EXPECT_TRUE(contains(report, "c2"));
    EXPECT_TRUE(contains(report, "c4"));
    EXPECT_FALSE(contains(report, "c1 "));
    EXPECT_LT(report.find("c5"), report.find("c2"));
    EXPECT_LT(report.find("c2"), report.find("c4"));
}

TEST(Reports, LargestCommitsWithHugeLimitShowsEverything) {
    Repository repo("demo");
    fillSampleRepository(repo);
    const std::string report = LargestCommitsReport(100).generate(repo);
    for (const char* id : {"c1", "c2", "c3", "c4", "c5", "c6", "c7"}) {
        EXPECT_TRUE(contains(report, id)) << id;
    }
}

TEST(Reports, LargestCommitsDoesNotChangeRepositoryOrder) {
    Repository repo("demo");
    fillSampleRepository(repo);
    LargestCommitsReport(3).generate(repo);
    EXPECT_EQ(repo.commits().get(0).id(), "c1");
    EXPECT_EQ(repo.commits().get(6).id(), "c7");
}

TEST(Reports, LargestCommitsOnEmptyRepository) {
    Repository repo("empty");
    EXPECT_TRUE(contains(LargestCommitsReport(3).generate(repo), "(no commits)"));
}

TEST(Reports, PullRequestSummaryCountsStates) {
    Repository repo("demo");
    fillSampleRepository(repo);
    const std::string report = PullRequestSummaryReport().generate(repo);
    EXPECT_TRUE(contains(report, "total 2 (open 1, merged 1, closed 0)"));
    EXPECT_TRUE(contains(report, "#2 Fix typo (fix-typo -> main)"));
    EXPECT_FALSE(contains(report, "#1 Add sorting"));  // змерджений не в списку відкритих
}

TEST(Reports, WorkThroughCommonInterface) {
    Repository repo("demo");
    fillSampleRepository(repo);

    std::vector<std::unique_ptr<IReport>> reports;
    reports.push_back(std::make_unique<CommitsPerAuthorReport>());
    reports.push_back(std::make_unique<LargestCommitsReport>(3));
    reports.push_back(std::make_unique<PullRequestSummaryReport>());

    for (const auto& report : reports) {
        const std::string text = report->generate(repo);
        EXPECT_TRUE(contains(text, report->name())) << report->name();
    }
}