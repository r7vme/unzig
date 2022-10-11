#!/bin/bash
set -e

./spec/validate.sh
#./codegen/generate.py

export CC=$(which clang)
export CXX=$(which clang++)
[ -d build ] || mkdir build
cmake -Bbuild
cmake --build build --parallel 16
cmake --build build --target test
