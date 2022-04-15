cmake_minimum_required(VERSION 3.10)
project(unzig)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)

# LLVM
find_package(LLVM REQUIRED CONFIG)
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

include_directories(src)

add_library(unzig src/parser.cpp src/tokenizer.cpp)
target_link_libraries(unzig PRIVATE LLVM)

add_executable(cmd src/cmd.cpp)
target_link_libraries(cmd unzig)

# tests
add_subdirectory(third_party/Catch2)
add_executable(tests test/test_tokenizer.cpp)
target_link_libraries(tests PRIVATE unzig Catch2::Catch2WithMain)
