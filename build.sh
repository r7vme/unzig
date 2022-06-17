#!/bin/bash
set -e

[ -d build ] || mkdir build
cmake -Bbuild
cmake --build build --parallel 16

# tests
./build/tests

# compile
cp spec/syntax.zig build
./build/unzig build/syntax.zig || true
cat ./build/syntax.ll
