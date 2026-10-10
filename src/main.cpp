#include <iostream>

#include "reposort/ConsoleApp.h"
#include "reposort/domain/Repository.h"
#include "reposort/domain/SampleRepository.h"

int main() {
    Repository repository("demo-repository");
    fillSampleRepository(repository);

    ConsoleApp app(std::cin, std::cout, repository);
    app.run();
    return 0;
}