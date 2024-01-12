#!/usr/bin/env bash

#
# Static code analysis using cppcheck
#
# Usage:
# $ ci/cppcheck.sh
#

echo -e "\u001b[34;1mStarting cppcheck...\u001b[0m"

cd "$(dirname "$0")/.."
cppcheck --enable=all --addon=misra --suppressions-list=ci/suppressions.txt --force --inline-suppr --error-exitcode=1 \
 -j 10 --std=c++17 --xml --xml-version=2 2>report.xml -I ./inc ./src -i ./src/Platform/ -i ./inc/Platform/
find . -type f -name '*.ctu-info' -delete

