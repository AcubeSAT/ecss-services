#!/usr/bin/env bash

#
# Edit files, performing code style corrections using clang-format
#
# Usage:
# $ ci/clang-format.sh
#

echo -e "\033[0;34mRunning clang-format...\033[0m"

cd "$(dirname "$0")" || exit
clang-format -style=file -i "$(find ../src/ ../inc/ ../test/ -type f -regextype posix-egrep -regex '.*\.(cpp|hpp|tpp|c|h)')" \
    -verbose "$@"
