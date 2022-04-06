cmake_minimum_required(VERSION 3.10)
project(unzig)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# LLVM
find_package(LLVM REQUIRED CONFIG)
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

include_directories(src)

add_library(unzig src/tokenizer.cpp)

add_executable(parser src/parser.cpp)
target_link_libraries(parser PRIVATE LLVM)

add_executable(interpreter src/interpreter.cpp)
target_link_libraries(interpreter unzig)

# tests
add_subdirectory(third_party/Catch2)
add_executable(tests test/test_tokenizer.cpp)
target_link_libraries(tests PRIVATE unzig Catch2::Catch2WithMain)
