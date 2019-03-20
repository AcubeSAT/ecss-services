#!/usr/bin/env bash

#
# Static code analysis for MISRA C-2012 compliance using cppcheck
#
# Usage:
# $ ci/cppcheck-misra.sh
#

echo -e "\u001b[34;1mRunning cppcheck with MISRA C(2012) rule compliance tests...\u001b[0m"

# grab the MISRA addon and the cppcheck addon interface from github
curl https://raw.githubusercontent.com/danmar/cppcheck/f4b5b156d720c712f6ce99f6e01d8c1b3f800d52/addons/misra.py > misra.py
curl https://raw.githubusercontent.com/danmar/cppcheck/f4b5b156d720c712f6ce99f6e01d8c1b3f800d52/addons/cppcheckdata.py > cppcheckdata.py

# generate dump files (XML representations of AST etc.) for all headers, source files etc.
#for file in $(find inc/ src/ -type f)
#do 
#    cppcheck --dump $file
#done

# run the MISRA checks against the dumps and send the results to a file
for file in $(find inc/ src/ -type f -name "*.dump")
do 
    python misra.py $file >> ci/report.msr 2>&1
done

# clean up the report file from any useless info
sed -i -r 's/(.*Script.*)|(.*Checking.*)|(.*MISRA.*)//gm' ci/report.msr
sed -i -r '/(^$)/d' ci/report.msr
sed -i -r 's/(\s\(.*\)\s)//gm' ci/report.msr
