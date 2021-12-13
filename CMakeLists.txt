cmake_minimum_required(VERSION 3.10)
project(unzig)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(LLVM REQUIRED CONFIG)
add_subdirectory(third_party/Catch2)

add_library(unzig src/tokenizer.cpp)
target_include_directories(unzig PUBLIC src)

# tests
add_executable(tests test/test_tokenizer.cpp)
target_link_libraries(tests PRIVATE unzig Catch2::Catch2WithMain)
