#!/bin/bash
set -e
export CC=$(which clang)
export CXX=$(which clang++)
[ -d build ] || mkdir build
cmake -Bbuild
cmake --build build --parallel 16
sudo cmake --build build --target install
# run tests
./build/tests
# test example
cp spec/syntax.uz build
./build/unzig build/syntax.uz
