#!/bin/bash
set -e

DIR="$(cd -P "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

hash peg || exit 1
hash make || exit 1

cd $DIR
make
./build/parser_debug < syntax.uz
