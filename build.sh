#!/bin/bash

LLVM_FLAGS=$(llvm-config --cxxflags --ldflags --system-libs --libs core)
clang++ -g -O3 src/unzig.cpp ${LLVM_FLAGS} -o unzig
