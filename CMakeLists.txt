cmake_minimum_required(VERSION 2.8.11)
project(unzig)

find_package(LLVM REQUIRED CONFIG)

separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
llvm_map_components_to_libnames(llvm_libs core)

include_directories(include)

add_executable(unzigc src/unzig.cpp src/lexer.cpp)
target_include_directories(unzigc SYSTEM PRIVATE ${LLVM_INCLUDE_DIRS})
target_compile_definitions(unzigc PRIVATE ${LLVM_DEFINITIONS_LIST})
target_link_libraries(unzigc ${llvm_libs})
