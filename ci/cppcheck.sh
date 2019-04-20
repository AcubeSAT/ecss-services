#!/usr/bin/env bash

#
# Static code analysis using cppcheck
#
# Usage:
# $ ci/cppcheck.sh
#

echo -e "\u001b[34;1mStarting cppcheck...\u001b[0m"

echo -e "\u001b[34;1mRunning cppcheck with default checklist...\u001b[0m"

cd "$(dirname "$0")/.."
cppcheck --enable=all --inline-suppr --suppress=unusedFunction --suppress=missingIncludeSystem \
    --suppress=noExplicitConstructor --error-exitcode=1 -I inc src test
