#!/usr/bin/env bash

#
# Static code analysis using cppchecl
#
# Usage:
# $ ci/cppcheck.sh
#

echo -e "\033[0;34mRunning cppcheck...\033[0m"

cd "$(dirname "$0")/.."
cppcheck --enable=all --inline-suppr --suppress=unusedFunction --suppress=missingIncludeSystem \
    --suppress=noExplicitConstructor --error-exitcode=1 -I inc src test
