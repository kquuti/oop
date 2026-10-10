#pragma once

#include <iosfwd>
#include <memory>
#include <optional>
#include <random>
#include <string>
#include <vector>

#include "reposort/ComparatorChain.h"
#include "reposort/IList.h"
#include "reposort/SortRegistry.h"
#include "reposort/domain/Commit.h"
#include "reposort/domain/Reports.h"
#include "reposort/domain/Repository.h"
#include "reposort/domain/Rules.h"

/// Консольне меню для роботи з репозиторієм.
/// Потоки введення й виведення передаються ззовні, тому клас можна перевіряти тестами.
class ConsoleApp {
public:
    /// seed задає зерно генератора випадкових комітів (однакове зерно дає однакові дані).
    ConsoleApp(std::istream& in, std::ostream& out, Repository& repository,
               unsigned int seed = 2025);

    ConsoleApp(const ConsoleApp&) = delete;
    ConsoleApp& operator=(const ConsoleApp&) = delete;

    /// Показує меню й виконує команди, поки користувач не вибере вихід
    /// або не закінчиться введення.
    void run();

    /// Виконує одну команду меню. Повертає false, якщо це команда виходу.
    /// Помилки (винятки) перехоплюються й виводяться як повідомлення.
    bool handleCommand(const std::string& command);

private:
    void printMenu();
    void showSummary();
    void listCommits();
    void sortCommits();
    void addRandomCommits();
    void checkRules();
    void showReports();
    void demonstrateTypes();

    void printCommits(const IList<Commit>& commits);

    // Читання вводу. Порожній optional означає, що введення закінчилось.
    std::optional<std::string> readLine(const std::string& prompt);
    std::optional<int> readNumber(const std::string& prompt, int min, int max);
    std::optional<bool> askYesNo(const std::string& prompt);
    std::optional<std::string> chooseAlgorithm();
    std::optional<ComparatorChain<Commit>> readCriteria();

    std::istream& in_;
    std::ostream& out_;
    Repository& repo_;
    std::mt19937 generator_;
    SortRegistry<Commit> registry_;
    RuleChecker rules_;
    std::vector<std::unique_ptr<IReport>> reports_;
};