#!/usr/bin/env bash

#
# Static code analysis using cppcheck
#
# Usage:
# $ ci/cppcheck.sh
#

echo -e "\033[0;34mStarting cppcheck...\033[0m"

echo -e "\033[0;34mRunning cppcheck with default checklist...\033[0m"

cd "$(dirname "$0")/.."
cppcheck --enable=all --inline-suppr --suppress=unusedFunction --suppress=missingIncludeSystem \
    --suppress=noExplicitConstructor --error-exitcode=1 -I inc src test

echo -e "\033[33mRunning cppcheck with MISRA rule compliance tests...\033[0m"

# grab the MISRA addon and the cppcheck addon interface from github
curl https://raw.githubusercontent.com/danmar/cppcheck/master/addons/misra.py > misra.py
curl https://raw.githubusercontent.com/danmar/cppcheck/master/addons/cppcheckdata.py > cppcheckdata.py

# generate dump files (XML representations of AST etc.) for all headers, source files etc.
for file in $(find inc/ src/ -type f)
do 
    cppcheck --dump $file
done

# run the MISRA checks against the dumps
for file in $(find inc/ src/ -type f -name "*.dump")
do 
    python misra.py $file
done