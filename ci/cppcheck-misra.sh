#!/usr/bin/env bash

#
# Static code analysis for MISRA C-2012 compliance using cppcheck
#
# Usage:
# $ ci/cppcheck-misra.sh
#

echo -e "\033[0;34mStarting cppcheck...\033[0m"

echo -e "\033[0;34mRunning cppcheck with MISRA C(2012) rule compliance tests...\033[0m"

# grab the MISRA addon and the cppcheck addon interface from github
curl https://raw.githubusercontent.com/danmar/cppcheck/f4b5b156d720c712f6ce99f6e01d8c1b3f800d52/addons/misra.py > misra.py
curl https://raw.githubusercontent.com/danmar/cppcheck/f4b5b156d720c712f6ce99f6e01d8c1b3f800d52/addons/cppcheckdata.py > cppcheckdata.py

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