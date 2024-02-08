#!/usr/bin/env bash

#
# Code style checks using clang-tidy
#
# Usage:
# $ ci/clang-tidy.sh <build-directory>
#

if [ -z "$1" ]; then
    echo "Usage: $0 <build-directory>"
    exit 1
fi

echo -e "\033[0;34mRunning clang-tidy...\033[0m"

clang-tidy -p "$1"/compile_commands.json --config-file=$(pwd)/.clang-tidy --use-color `find src inc -not \
-path "*/Platform/*" -type f -regextype posix-egrep -regex '.*\.(cpp|hpp|c|h)'` | tee clang-tidy-output.log
