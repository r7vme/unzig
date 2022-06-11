#!/bin/bash
set -e
set -o pipefail

DIR="$(cd -P "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

fail() {
  printf "\033[1;31merror: %s\033[0m\n" "${1:-"Unknown error"}"
  exit 1
}

msg() {
  printf "\033[1;32m%s\033[0m\n" "$1"
}

hash dot || fail "please install graphviz dot"
[[ -d ${DIR}/../build ]] || fail "please build first"
(cat ${DIR}/../spec/syntax.zig | ${DIR}/../build/unzig_dotgen | dot -Tpng > /dev/null) || fail "test_dotgen.sh FAIL"
msg "test_dotgen.sh OK"
