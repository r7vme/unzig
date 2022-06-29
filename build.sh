#!/bin/bash
set -e
[ -d build ] || mkdir build
cmake -Bbuild
cmake --build build --parallel 16
sudo cmake --build build --target install
# run tests
./build/tests
# test example
cp spec/syntax.uz build
./build/unzig build/syntax.uz
