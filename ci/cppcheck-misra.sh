#!/usr/bin/env bash

#
# Static code analysis for MISRA C-2012 compliance using cppcheck
#
# Usage:
# $ ci/cppcheck-misra.sh
#

# make sure we are in the correct directory, regardless of where the script was called from
cd "$(dirname "$0")/.."

echo -e "\u001b[34;1mGetting prerequisites...\u001b[0m"
# grab the MISRA addon and the cppcheck addon interface from github
curl https://raw.githubusercontent.com/danmar/cppcheck/f4b5b156d720c712f6ce99f6e01d8c1b3f800d52/addons/misra.py > ci/misra.py
curl https://raw.githubusercontent.com/danmar/cppcheck/f4b5b156d720c712f6ce99f6e01d8c1b3f800d52/addons/cppcheckdata.py > ci/cppcheckdata.py

# clean up old files
echo -e "\u001b[34;1mRemoving old files...\u001b[0m"
echo > ci/report.msr # clear the report file
find inc/ src/ -type f -name "*.dump" | xargs rm

# generate dump files (XML representations of AST etc.) for all headers, source files etc.
echo -e "\u001b[34;1mGenerating dump files...\u001b[0m"
find inc/ src/ -type f \( -iname "*.cpp" -or -iname "*.hpp" \) | xargs cppcheck --dump

# run the MISRA checks against the dumps and send the results to a file
echo -e "\u001b[34;1mRunning MISRA C(2012) rule compliance tests...\u001b[0m"
find inc/ src/ -type f -name "*.dump" | xargs python3 ci/misra.py >> ci/report.msr 2>&1

# pre-process the generated report to remove all useless strings
echo -e "\u001b[34;1mPre-processing report...\u001b[0m"
sed -i -r 's/(.*Script.*)|(.*Checking.*)|(.*MISRA.*)//gm; /(^$)/d; s/(\s\(.*\)\s)//gm; s/(\]|\[)//gm; s/(misra-c2012-)//gm' ci/report.msr

# run the summarizer for a nice, clean summary of errors
echo -e "\u001b[34;1mSummarizing results...\u001b[0m"
python3 ci/summarizer.py --report ci/report.msr --suppress 3.1 5.1 5.2 5.3 12.3 14.4 15.5 16.3 18.8

