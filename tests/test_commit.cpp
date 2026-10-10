#include <gtest/gtest.h>

#include <string>

#include "reposort/domain/Commit.h"

TEST(Commit, DefaultCommitIsEmpty) {
    Commit commit;
    EXPECT_EQ(commit.id(), "");
    EXPECT_EQ(commit.author(), "");
    EXPECT_EQ(commit.timestamp(), 0);
    EXPECT_EQ(commit.linesChanged(), 0);
}

TEST(Commit, StoresValues) {
    Commit commit("a1", "Olena", 100, "Fix bug", 10);
    EXPECT_EQ(commit.id(), "a1");
    EXPECT_EQ(commit.author(), "Olena");
    EXPECT_EQ(commit.timestamp(), 100);
    EXPECT_EQ(commit.message(), "Fix bug");
    EXPECT_EQ(commit.linesChanged(), 10);
}

TEST(Commit, IsLargeUsesThreshold) {
    Commit commit("a1", "Olena", 100, "Fix bug", 50);
    EXPECT_TRUE(commit.isLarge(50));   // межа включається
    EXPECT_TRUE(commit.isLarge(10));
    EXPECT_FALSE(commit.isLarge(51));
}

TEST(Commit, ShortMessageKeepsShortText) {
    Commit commit("a1", "Olena", 100, "Fix bug", 10);
    EXPECT_EQ(commit.shortMessage(20), "Fix bug");
    EXPECT_EQ(commit.shortMessage(7), "Fix bug");  // рівно по довжині
}

TEST(Commit, ShortMessageTruncatesLongText) {
    Commit commit("a1", "Olena", 100, "Implement sorting", 10);
    EXPECT_EQ(commit.shortMessage(10), "Impleme...");
}

TEST(Commit, ShortMessageWithTinyLimit) {
    Commit commit("a1", "Olena", 100, "Implement sorting", 10);
    EXPECT_EQ(commit.shortMessage(2), "Im");
}

TEST(Commit, ToStringContainsMainFields) {
    Commit commit("a1", "Olena", 100, "Fix bug", 10);
    const std::string text = commit.toString();
    EXPECT_NE(text.find("a1"), std::string::npos);
    EXPECT_NE(text.find("Olena"), std::string::npos);
    EXPECT_NE(text.find("Fix bug"), std::string::npos);
}