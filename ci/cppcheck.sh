#!/usr/bin/env bash

#
# Static code analysis using cppchecl
#
# Usage:
# $ ci/cppcheck.sh
#

cd "$(dirname "$0")/.."
cppcheck --enable=all --error-exitcode=1 -I inc src
