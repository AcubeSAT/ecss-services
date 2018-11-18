#!/usr/bin/env bash

#
# Static code analysis using cppchecl
#
# Usage:
# $ ci/cppcheck.sh
#

echo -e "\033[0;34mRunning cppcheck...\033[0m"

cd "$(dirname "$0")/.."
cppcheck --enable=all --error-exitcode=1 -I inc src tests
