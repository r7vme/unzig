#!/bin/bash
set -e

[ -d build ] || mkdir build
cmake -Bbuild
cmake --build build

# tests
./build/tests

# compile
cp spec/syntax.zig build
./build/unzig build/syntax.zig
