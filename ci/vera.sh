#!/usr/bin/env bash

#
# Code style checks using vera++
#
# Usage:
# $ ci/vera.sh
#

echo -e "\033[0;34mRunning vera++...\033[0m"

cd "$(dirname "$0")/.."
vera++ --error --parameter max-line-length=120 --profile custom `find src inc test -type f -regextype posix-egrep -regex '.*\.(cpp|hpp|c|h)'`
