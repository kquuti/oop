#include <gtest/gtest.h>

#include <stdexcept>
#include <string>

#include "reposort/domain/Author.h"
#include "reposort/domain/Branch.h"
#include "reposort/domain/PullRequest.h"
#include "reposort/domain/Repository.h"
#include "reposort/domain/SampleRepository.h"

// ---------- Author ----------

TEST(Author, StoresValues) {
    Author author("Olena", "olena@example.com");
    EXPECT_EQ(author.name(), "Olena");
    EXPECT_EQ(author.email(), "olena@example.com");
}

TEST(Author, ValidEmails) {
    EXPECT_TRUE(Author("a", "a@b.c").isValidEmail());
    EXPECT_TRUE(Author("a", "olena@example.com").isValidEmail());
}

TEST(Author, InvalidEmails) {
    EXPECT_FALSE(Author("a", "").isValidEmail());
    EXPECT_FALSE(Author("a", "no-at-sign").isValidEmail());
    EXPECT_FALSE(Author("a", "@example.com").isValidEmail());
    EXPECT_FALSE(Author("a", "a@@b.c").isValidEmail());
    EXPECT_FALSE(Author("a", "a@b").isValidEmail());
    EXPECT_FALSE(Author("a", "a@.c").isValidEmail());
    EXPECT_FALSE(Author("a", "a@b.").isValidEmail());
}

TEST(Author, EmailDomain) {
    EXPECT_EQ(Author("a", "olena@example.com").emailDomain(), "example.com");
    EXPECT_EQ(Author("a", "broken").emailDomain(), "");
}

// ---------- Branch ----------

TEST(Branch, StartsEmpty) {
    Branch branch("main");
    EXPECT_EQ(branch.name(), "main");
    EXPECT_EQ(branch.commitCount(), 0u);
}

TEST(Branch, AddCommitKeepsOrderAndIgnoresDuplicates) {
    Branch branch("main");
    branch.addCommit("c1");
    branch.addCommit("c2");
    branch.addCommit("c1");
    EXPECT_EQ(branch.commitCount(), 2u);
    EXPECT_EQ(branch.commitIds()[0], "c1");
    EXPECT_EQ(branch.commitIds()[1], "c2");
    EXPECT_TRUE(branch.containsCommit("c2"));
    EXPECT_FALSE(branch.containsCommit("c3"));
}

TEST(Branch, IsMain) {
    EXPECT_TRUE(Branch("main").isMain());
    EXPECT_TRUE(Branch("master").isMain());
    EXPECT_FALSE(Branch("develop").isMain());
}

TEST(Branch, ConventionalNames) {
    for (const char* name : {"main", "master", "develop", "feature/sorting", "bugfix/x",
                             "hotfix/urgent", "release/1.0"}) {
        EXPECT_TRUE(Branch(name).hasConventionalName()) << name;
    }
}

TEST(Branch, UnconventionalNames) {
    for (const char* name : {"", "fix-typo", "feature/", "Feature/x", "features/x", "mybranch"}) {
        EXPECT_FALSE(Branch(name).hasConventionalName()) << name;
    }
}

// ---------- PullRequest ----------

TEST(PullRequest, StartsOpen) {
    PullRequest pr(1, "Title", "Olena", "feature/x", "main");
    EXPECT_EQ(pr.id(), 1);
    EXPECT_EQ(pr.title(), "Title");
    EXPECT_EQ(pr.author(), "Olena");
    EXPECT_EQ(pr.sourceBranch(), "feature/x");
    EXPECT_EQ(pr.targetBranch(), "main");
    EXPECT_TRUE(pr.isOpen());
}

TEST(PullRequest, MergeChangesState) {
    PullRequest pr(1, "T", "a", "s", "t");
    pr.merge();
    EXPECT_EQ(pr.state(), PullRequestState::Merged);
    EXPECT_FALSE(pr.isOpen());
}

TEST(PullRequest, CloseChangesState) {
    PullRequest pr(1, "T", "a", "s", "t");
    pr.close();
    EXPECT_EQ(pr.state(), PullRequestState::Closed);
}

TEST(PullRequest, CannotChangeStateTwice) {
    PullRequest pr(1, "T", "a", "s", "t");
    pr.merge();
    EXPECT_THROW(pr.merge(), std::logic_error);
    EXPECT_THROW(pr.close(), std::logic_error);
}

// ---------- Repository ----------

TEST(Repository, NewRepositoryIsEmpty) {
    Repository repo("demo");
    EXPECT_EQ(repo.name(), "demo");
    EXPECT_EQ(repo.commitCount(), 0u);
    EXPECT_TRUE(repo.branches().empty());
    EXPECT_TRUE(repo.authors().empty());
    EXPECT_TRUE(repo.pullRequests().empty());
}

TEST(Repository, AddsAndFindsAuthors) {
    Repository repo("demo");
    repo.addAuthor(Author("Olena", "o@example.com"));
    ASSERT_NE(repo.findAuthor("Olena"), nullptr);
    EXPECT_EQ(repo.findAuthor("Olena")->email(), "o@example.com");
    EXPECT_EQ(repo.findAuthor("Nobody"), nullptr);
    EXPECT_THROW(repo.addAuthor(Author("Olena", "other@example.com")), std::invalid_argument);
}

TEST(Repository, AddsAndFindsCommits) {
    Repository repo("demo");
    repo.addCommit(Commit("c1", "Olena", 100, "Message", 10));
    EXPECT_EQ(repo.commitCount(), 1u);
    ASSERT_NE(repo.findCommit("c1"), nullptr);
    EXPECT_EQ(repo.findCommit("c1")->author(), "Olena");
    EXPECT_EQ(repo.findCommit("zzz"), nullptr);
    EXPECT_THROW(repo.addCommit(Commit("c1", "Ivan", 200, "Other", 5)), std::invalid_argument);
    EXPECT_EQ(repo.commitCount(), 1u);
}

TEST(Repository, CommitsByAuthor) {
    Repository repo("demo");
    fillSampleRepository(repo);
    const std::vector<Commit> olena = repo.commitsByAuthor("Olena");
    ASSERT_EQ(olena.size(), 3u);
    EXPECT_EQ(olena[0].id(), "c1");
    EXPECT_EQ(olena[1].id(), "c3");
    EXPECT_EQ(olena[2].id(), "c7");
    EXPECT_TRUE(repo.commitsByAuthor("Nobody").empty());
}

TEST(Repository, CopyCommitsToAnyList) {
    Repository repo("demo");
    fillSampleRepository(repo);
    ArrayList<Commit> target;
    repo.copyCommitsTo(target);
    EXPECT_EQ(target.size(), repo.commitCount());
    EXPECT_EQ(target.get(0).id(), "c1");
}

TEST(Repository, AddsBranchesAndCommitsToThem) {
    Repository repo("demo");
    repo.addCommit(Commit("c1", "Olena", 100, "Message", 10));
    repo.addBranch(Branch("main"));
    EXPECT_THROW(repo.addBranch(Branch("main")), std::invalid_argument);

    repo.addCommitToBranch("main", "c1");
    ASSERT_NE(repo.findBranch("main"), nullptr);
    EXPECT_EQ(repo.findBranch("main")->commitCount(), 1u);

    EXPECT_THROW(repo.addCommitToBranch("nope", "c1"), std::invalid_argument);
    EXPECT_THROW(repo.addCommitToBranch("main", "nope"), std::invalid_argument);
    EXPECT_EQ(repo.findBranch("nope"), nullptr);
}

TEST(Repository, PullRequestNeedsExistingBranches) {
    Repository repo("demo");
    repo.addBranch(Branch("main"));
    EXPECT_THROW(repo.addPullRequest(PullRequest(1, "T", "a", "feature/x", "main")),
                 std::invalid_argument);
    repo.addBranch(Branch("feature/x"));
    repo.addPullRequest(PullRequest(1, "T", "a", "feature/x", "main"));
    EXPECT_EQ(repo.pullRequests().size(), 1u);
    EXPECT_THROW(repo.addPullRequest(PullRequest(1, "Again", "a", "feature/x", "main")),
                 std::invalid_argument);
}

TEST(Repository, MergesAndClosesPullRequests) {
    Repository repo("demo");
    repo.addBranch(Branch("main"));
    repo.addBranch(Branch("feature/x"));
    repo.addPullRequest(PullRequest(1, "T", "a", "feature/x", "main"));
    repo.addPullRequest(PullRequest(2, "U", "a", "feature/x", "main"));

    repo.mergePullRequest(1);
    repo.closePullRequest(2);
    EXPECT_EQ(repo.findPullRequest(1)->state(), PullRequestState::Merged);
    EXPECT_EQ(repo.findPullRequest(2)->state(), PullRequestState::Closed);

    EXPECT_THROW(repo.mergePullRequest(1), std::logic_error);
    EXPECT_THROW(repo.mergePullRequest(99), std::invalid_argument);
    EXPECT_EQ(repo.findPullRequest(99), nullptr);
}

TEST(Repository, SampleRepositoryHasExpectedContent) {
    Repository repo("demo");
    fillSampleRepository(repo);
    EXPECT_EQ(repo.authors().size(), 3u);
    EXPECT_EQ(repo.commitCount(), 7u);
    EXPECT_EQ(repo.branches().size(), 3u);
    EXPECT_EQ(repo.pullRequests().size(), 2u);
    EXPECT_EQ(repo.findBranch("main")->commitCount(), 4u);
    EXPECT_EQ(repo.findPullRequest(1)->state(), PullRequestState::Merged);
    EXPECT_TRUE(repo.findPullRequest(2)->isOpen());
}