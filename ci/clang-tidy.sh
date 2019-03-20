#!/usr/bin/env bash

#
# Code style checks using clang-tidy
#
# Usage:
# $ ci/clang-tidy.sh
#

echo -e "\033[0;34mRunning clang-tidy...\033[0m"

cd "$(dirname "$0")"
clang-tidy-7 `find ../src/ -type f -regextype posix-egrep -regex '.*\.(cpp|hpp|c|h)'` \
    -extra-arg=-fcolor-diagnostics  -- -std=c++17 -I../inc \
    -I/usr/include/c++/7/ -I/usr/include/x86_64-linux-gnu/c++/7 -I../lib/etl/include
