#!/bin/bash

# to llvm
clang -S -emit-llvm 1.c

# llvm to binary
clang 2.ll -o 2
