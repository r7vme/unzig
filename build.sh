#!/bin/bash
set -e

[ -d build ] || mkdir build
cmake -Bbuild
cmake --build build
