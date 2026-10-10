#pragma once

#include "reposort/domain/Repository.h"

/// Заповнює репозиторій тестовими даними:
/// 3 автори (+ коміт невідомого автора Petro), 7 комітів, 3 гілки, 2 pull request
/// (перший змерджено). Дані містять навмисні порушення правил.
inline void fillSampleRepository(Repository& repo) {
    repo.addAuthor(Author("Olena", "olena@example.com"));
    repo.addAuthor(Author("Ivan", "ivan@example.com"));
    repo.addAuthor(Author("Maria", "maria@example.com"));

    repo.addCommit(Commit("c1", "Olena", 1700000000, "Initial commit", 120));
    repo.addCommit(Commit("c2", "Ivan", 1700003600, "Add list classes", 340));
    repo.addCommit(Commit("c3", "Olena", 1700007200, "Fix", 4));
    repo.addCommit(Commit("c4", "Maria", 1700010800, "Implement merge sort", 210));
    repo.addCommit(Commit("c5", "Ivan", 1700014400, "Add sorting registry and tests", 520));
    repo.addCommit(Commit("c6", "Petro", 1700018000, "Update documentation", 35));
    repo.addCommit(Commit("c7", "Olena", 1700021600, "Refactor comparators", 90));

    repo.addBranch(Branch("main"));
    repo.addBranch(Branch("feature/sorting"));
    repo.addBranch(Branch("fix-typo"));

    for (const char* id : {"c1", "c2", "c3", "c4"}) {
        repo.addCommitToBranch("main", id);
    }
    repo.addCommitToBranch("feature/sorting", "c5");
    repo.addCommitToBranch("feature/sorting", "c7");
    repo.addCommitToBranch("fix-typo", "c6");

    repo.addPullRequest(PullRequest(1, "Add sorting", "Ivan", "feature/sorting", "main"));
    repo.addPullRequest(PullRequest(2, "Fix typo", "Petro", "fix-typo", "main"));
    repo.mergePullRequest(1);
};