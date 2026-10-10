#include "reposort/ConsoleApp.h"

#include <algorithm>
#include <cctype>
#include <chrono>
#include <exception>
#include <istream>
#include <memory>
#include <optional>
#include <ostream>
#include <string>
#include <vector>

#include "reposort/ArrayList.h"
#include "reposort/LinkedList.h"
#include "reposort/RandomData.h"
#include "reposort/StandardSorts.h"
#include "reposort/VectorList.h"
#include "reposort/domain/RandomCommit.h"

namespace {

constexpr std::size_t kMinMessageLength = 10;
constexpr int kMaxCommitLines = 500;
constexpr std::size_t kTopCommits = 5;
constexpr std::size_t kMaxShownCommits = 20;
constexpr int kMaxRandomCommits = 1000;
constexpr std::size_t kDemoSize = 5;

/// Прибирає пробіли та переноси рядків з обох країв.
std::string trim(const std::string& text) {
    const char* spaces = " \t\r\n";
    const std::size_t first = text.find_first_not_of(spaces);
    if (first == std::string::npos) {
        return "";
    }
    const std::size_t last = text.find_last_not_of(spaces);
    return text.substr(first, last - first + 1);
}

/// Перетворює рядок на число. Порожній результат, якщо рядок не є числом повністю.
std::optional<long> parseNumber(const std::string& text) {
    const std::string trimmed = trim(text);
    if (trimmed.empty()) {
        return std::nullopt;
    }
    try {
        std::size_t used = 0;
        const long value = std::stol(trimmed, &used);
        if (used != trimmed.size()) {
            return std::nullopt;
        }
        return value;
    } catch (const std::exception&) {
        return std::nullopt;  // не число або завелике число
    }
}

/// Критерій сортування за номером поля: 1 автор, 2 час, 3 рядки, 4 id.
Comparator<Commit> makeCriterion(int field, bool descending) {
    Comparator<Commit> criterion;
    switch (field) {
        case 1:
            criterion = comparingBy<Commit>(&Commit::author);
            break;
        case 2:
            criterion = comparingBy<Commit>(&Commit::timestamp);
            break;
        case 3:
            criterion = comparingBy<Commit>(&Commit::linesChanged);
            break;
        default:
            criterion = comparingBy<Commit>(&Commit::id);
            break;
    }
    return descending ? reversed(criterion) : criterion;
}

/// Створює порожній список за номером: 1 ArrayList, 2 LinkedList, інакше VectorList.
std::unique_ptr<IList<Commit>> createList(int choice) {
    switch (choice) {
        case 1:
            return std::make_unique<ArrayList<Commit>>();
        case 2:
            return std::make_unique<LinkedList<Commit>>();
        default:
            return std::make_unique<VectorList<Commit>>();
    }
}

std::string listName(int choice) {
    switch (choice) {
        case 1:
            return "ArrayList";
        case 2:
            return "LinkedList";
        default:
            return "VectorList";
    }
}

/// Заповнює список випадковими елементами типу T, сортує обраним алгоритмом
/// і друкує до та після.
template <typename T>
void demonstrateType(std::ostream& out, const std::string& title, const std::string& algorithmKey,
                     std::mt19937& generator) {
    ArrayList<T> list;
    fillRandom(list, kDemoSize, generator);
    out << title << ":\n  before: " << list.toString() << "\n";

    SortRegistry<T> registry;
    registerStandardSorts(registry);
    const std::unique_ptr<ISortStrategy<T>> sorter = registry.create(algorithmKey);
    sorter->sort(list, naturalOrder<T>());
    out << "  after (" << sorter->name() << "): " << list.toString() << "\n";
}

}  // namespace

ConsoleApp::ConsoleApp(std::istream& in, std::ostream& out, Repository& repository,
                       unsigned int seed)
    : in_(in), out_(out), repo_(repository), generator_(seed) {
    registerStandardSorts(registry_);

    rules_.add(std::make_unique<CommitMessageRule>(kMinMessageLength));
    rules_.add(std::make_unique<CommitSizeRule>(kMaxCommitLines));
    rules_.add(std::make_unique<BranchNamingRule>());
    rules_.add(std::make_unique<KnownAuthorRule>());

    reports_.push_back(std::make_unique<CommitsPerAuthorReport>());
    reports_.push_back(std::make_unique<LargestCommitsReport>(kTopCommits));
    reports_.push_back(std::make_unique<PullRequestSummaryReport>());
}

void ConsoleApp::run() {
    out_ << "RepoSort: repository \"" << repo_.name() << "\"\n";
    bool running = true;
    while (running) {
        printMenu();
        const std::optional<std::string> line = readLine("> ");
        if (!line) {
            break;  // введення закінчилось
        }
        running = handleCommand(*line);
    }
    out_ << "Bye!\n";
}

bool ConsoleApp::handleCommand(const std::string& command) {
    const std::string trimmed = trim(command);
    try {
        if (trimmed == "0") {
            return false;
        }
        if (trimmed == "1") {
            showSummary();
        } else if (trimmed == "2") {
            listCommits();
        } else if (trimmed == "3") {
            sortCommits();
        } else if (trimmed == "4") {
            addRandomCommits();
        } else if (trimmed == "5") {
            checkRules();
        } else if (trimmed == "6") {
            showReports();
        } else if (trimmed == "7") {
            demonstrateTypes();
        } else {
            out_ << "Unknown command: " << trimmed << "\n";
        }
    } catch (const std::exception& error) {
        out_ << "Error: " << error.what() << "\n";
    }
    return true;
}

void ConsoleApp::printMenu() {
    out_ << "\n==== RepoSort ====\n"
         << "1. Repository summary\n"
         << "2. List commits\n"
         << "3. Sort commits\n"
         << "4. Add random commits\n"
         << "5. Check rules\n"
         << "6. Reports\n"
         << "7. Demo on different data types\n"
         << "0. Exit\n";
}

void ConsoleApp::showSummary() {
    out_ << "Repository: " << repo_.name() << "\n";
    out_ << "Authors: " << repo_.authors().size() << "\n";
    out_ << "Commits: " << repo_.commitCount() << "\n";
    out_ << "Branches: " << repo_.branches().size() << "\n";
    for (const Branch& branch : repo_.branches()) {
        out_ << "  " << branch.name() << " (" << branch.commitCount() << " commits)\n";
    }
    out_ << "Pull requests: " << repo_.pullRequests().size() << "\n";
}

void ConsoleApp::listCommits() {
    if (repo_.commitCount() == 0) {
        out_ << "No commits.\n";
        return;
    }
    printCommits(repo_.commits());
}

void ConsoleApp::printCommits(const IList<Commit>& commits) {
    const std::size_t shown = std::min(commits.size(), kMaxShownCommits);
    for (std::size_t i = 0; i < shown; ++i) {
        out_ << "  " << (i + 1) << ". " << commits.get(i).toString() << "\n";
    }
    if (commits.size() > shown) {
        out_ << "  ... and " << (commits.size() - shown) << " more\n";
    }
}

void ConsoleApp::sortCommits() {
    if (repo_.commitCount() == 0) {
        out_ << "No commits to sort.\n";
        return;
    }

    const std::optional<std::string> algorithmKey = chooseAlgorithm();
    if (!algorithmKey) {
        return;
    }

    out_ << "List types:\n  1. ArrayList\n  2. LinkedList\n  3. VectorList\n";
    const std::optional<int> listChoice = readNumber("List type: ", 1, 3);
    if (!listChoice) {
        return;
    }

    const std::optional<ComparatorChain<Commit>> criteria = readCriteria();
    if (!criteria) {
        return;
    }

    const std::unique_ptr<ISortStrategy<Commit>> sorter = registry_.create(*algorithmKey);
    const std::unique_ptr<IList<Commit>> list = createList(*listChoice);
    repo_.copyCommitsTo(*list);  // сортуємо копію, репозиторій не змінюється

    const auto start = std::chrono::steady_clock::now();
    sorter->sort(*list, *criteria);
    const auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::steady_clock::now() - start);

    out_ << "Sorted " << list->size() << " commits with " << sorter->name() << " on "
         << listName(*listChoice) << " in " << elapsed.count() << " microseconds\n";
    printCommits(*list);
}

void ConsoleApp::addRandomCommits() {
    const std::optional<int> count = readNumber("How many commits (1-1000): ", 1, kMaxRandomCommits);
    if (!count) {
        return;
    }
    int added = 0;
    int skipped = 0;
    for (int i = 0; i < *count; ++i) {
        const Commit commit = randomValue<Commit>(generator_);
        if (repo_.findCommit(commit.id()) != nullptr) {
            ++skipped;  // випадковий id збігся з існуючим
            continue;
        }
        repo_.addCommit(commit);
        ++added;
    }
    out_ << "Added " << added << " random commits";
    if (skipped > 0) {
        out_ << " (" << skipped << " skipped: duplicate ids)";
    }
    out_ << "\n";
}

void ConsoleApp::checkRules() {
    const std::vector<Violation> violations = rules_.checkAll(repo_);
    out_ << "Checked " << rules_.size() << " rules.\n";
    if (violations.empty()) {
        out_ << "No rule violations found.\n";
        return;
    }
    out_ << "Violations found: " << violations.size() << "\n";
    for (const Violation& violation : violations) {
        out_ << "  [" << violation.rule << "] " << violation.message << "\n";
    }
}

void ConsoleApp::showReports() {
    out_ << "Reports:\n";
    for (std::size_t i = 0; i < reports_.size(); ++i) {
        out_ << "  " << (i + 1) << ". " << reports_[i]->name() << "\n";
    }
    const std::optional<int> choice = readNumber("Report: ", 1, static_cast<int>(reports_.size()));
    if (!choice) {
        return;
    }
    out_ << reports_[static_cast<std::size_t>(*choice - 1)]->generate(repo_);
}

void ConsoleApp::demonstrateTypes() {
    const std::optional<std::string> algorithmKey = chooseAlgorithm();
    if (!algorithmKey) {
        return;
    }
    demonstrateType<int>(out_, "int", *algorithmKey, generator_);
    demonstrateType<double>(out_, "double", *algorithmKey, generator_);
    demonstrateType<std::string>(out_, "string", *algorithmKey, generator_);
    demonstrateType<std::vector<int>>(out_, "vector<int>", *algorithmKey, generator_);
    demonstrateType<Commit>(out_, "Commit", *algorithmKey, generator_);
}

std::optional<std::string> ConsoleApp::readLine(const std::string& prompt) {
    out_ << prompt << std::flush;
    std::string line;
    if (!std::getline(in_, line)) {
        return std::nullopt;
    }
    return trim(line);
}

std::optional<int> ConsoleApp::readNumber(const std::string& prompt, int min, int max) {
    while (true) {
        const std::optional<std::string> line = readLine(prompt);
        if (!line) {
            return std::nullopt;
        }
        const std::optional<long> value = parseNumber(*line);
        if (value && *value >= min && *value <= max) {
            return static_cast<int>(*value);
        }
        out_ << "Please enter a number from " << min << " to " << max << ".\n";
    }
}

std::optional<bool> ConsoleApp::askYesNo(const std::string& prompt) {
    while (true) {
        const std::optional<std::string> line = readLine(prompt);
        if (!line) {
            return std::nullopt;
        }
        std::string answer = *line;
        std::transform(answer.begin(), answer.end(), answer.begin(),
                       [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        if (answer == "y" || answer == "yes") {
            return true;
        }
        if (answer == "n" || answer == "no") {
            return false;
        }
        out_ << "Please answer y or n.\n";
    }
}

std::optional<std::string> ConsoleApp::chooseAlgorithm() {
    const std::vector<std::string> keys = registry_.keys();
    out_ << "Sorting algorithms:\n";
    for (std::size_t i = 0; i < keys.size(); ++i) {
        out_ << "  " << (i + 1) << ". " << registry_.create(keys[i])->name() << "\n";
    }
    const std::optional<int> choice = readNumber("Algorithm: ", 1, static_cast<int>(keys.size()));
    if (!choice) {
        return std::nullopt;
    }
    return keys[static_cast<std::size_t>(*choice - 1)];
}

std::optional<ComparatorChain<Commit>> ConsoleApp::readCriteria() {
    ComparatorChain<Commit> chain;
    while (true) {
        out_ << "Sort by: 1. author  2. time  3. lines changed  4. id\n";
        const std::optional<int> field = readNumber("Field: ", 1, 4);
        if (!field) {
            return std::nullopt;
        }
        out_ << "Direction: 1. ascending  2. descending\n";
        const std::optional<int> direction = readNumber("Direction: ", 1, 2);
        if (!direction) {
            return std::nullopt;
        }
        chain.add(makeCriterion(*field, *direction == 2));

        const std::optional<bool> more = askYesNo("Add another criterion? (y/n): ");
        if (!more) {
            return std::nullopt;
        }
        if (!*more) {
            break;
        }
    }
    // Останній критерій id робить порядок повним: результат однаковий для всіх алгоритмів.
    chain.add(comparingBy<Commit>(&Commit::id));
    return chain;
}