cmake_minimum_required(VERSION 3.14)
project(RepoSort LANGUAGES CXX)

# Стандарт мови C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Бібліотека зі скомпільованим (нешаблонним) кодом
add_library(reposort_core src/ConsoleApp.cpp)
target_include_directories(reposort_core PUBLIC include)

# Основна програма
add_executable(reposort src/main.cpp)
target_link_libraries(reposort PRIVATE reposort_core)

# Підключення GoogleTest (завантажується автоматично при першій збірці)
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Тести
enable_testing()
add_executable(unit_tests
  tests/test_smoke.cpp
  tests/test_lists.cpp
  tests/test_sorting.cpp
  tests/test_commit.cpp
  tests/test_comparators.cpp
  tests/test_registry.cpp
  tests/test_text_random.cpp
  tests/test_domain.cpp
  tests/test_rules_reports.cpp
  tests/test_console.cpp
)
target_link_libraries(unit_tests PRIVATE reposort_core GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(unit_tests)