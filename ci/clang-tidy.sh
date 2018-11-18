#!/usr/bin/env bash

#
# Code style checks using clang-tidy
#
# Usage:
# $ ci/clang-tidy.sh
#

echo -e "\033[0;34mRunning clang-tidy...\033[0m"

cd "$(dirname "$0")"
clang-tidy-4.0 `find ../src/ -type f -regextype posix-egrep -regex '.*\.(cpp|hpp|c|h)'` \
    -extra-arg=-fcolor-diagnostics -- -std=c++14 -I../inc
