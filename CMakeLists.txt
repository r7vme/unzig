cmake_minimum_required(VERSION 3.10)
project(unzig)

find_package(LLVM REQUIRED CONFIG)
find_package(Catch2 2 REQUIRED)

add_executable(unzigc src/main.cpp src/tokenizer.cpp)

# tests
add_executable(tests test/test.cpp)
target_link_libraries(tests Catch2::Catch2)
