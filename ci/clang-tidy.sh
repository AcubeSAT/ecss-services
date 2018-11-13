#!/usr/bin/env bash

#
# Code style checks using clang-tidy
#
# Usage:
# $ ci/clang-tidy.sh
#

cd "$(dirname "$0")"
clang-tidy `find ../src/ -type f -regextype posix-egrep -regex '.*\.(cpp|hpp|c|h)'` -- -std=c++11 -I../inc
