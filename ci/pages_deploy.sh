#!/usr/bin/env bash

# Bash color ouput
# https://misc.flogisoft.com/bash/tip_colors_and_formatting

# Variables used in gcovr
EXCLUDED_FILES="^.*(test|lib|main.cpp|CMakeFiles)"
HTML_TITLE="Code coverage ${CI_PROJECT_NAME}/${CI_COMMIT_REF_NAME}"

# Variables to use with lcov
cp ci/lcovrc ~/.lcovrc # The coverage configuration file resides in ~/ as ~/.lcovrc
PAGE_TITLE="${CI_COMMIT_REF_NAME}"

# Assign the correct folder names
if [[ ${CI_COMMIT_REF_NAME} == "master" ]];
then
    ROOT_PATH=""
    COVERAGE_PATH="coverage"
    DOCUMENT_PATH="docs"
else
    ROOT_PATH="${CI_COMMIT_REF_NAME}"
    COVERAGE_PATH="coverage/${CI_COMMIT_REF_NAME}"
    DOCUMENT_PATH="docs/${CI_COMMIT_REF_NAME}"
fi

# Empty the contents from the stored cache, if any
rm -rf public/${DOCUMENT_PATH}/*

# Create the necessary directories
mkdir -p public/${DOCUMENT_PATH}


# Try to make and build the application
cmake . -DCMAKE_CXX_FLAGS="-g -O0 --coverage" && make all -j4

# If the command above returned something different than zero, generate only the docs
if [[ $? -ne 0 ]];
then
    echo -e "\e[1;5;91mProgram build failed, only the documentation will be generated.\e[0m"
else
    # Coverage generation using lcov
    # Generate coverage baseline
    lcov -q --capture --initial --directory . -o coverage_base
    ./tests --use-colour yes # Run the tests to generate coverage notes

    # In the event of test failure, generate only the documentation
    if [[ $? -ne 0 ]];
    then
        echo -e "\e[1;5;91mTests failed, only documentation will be generated.\e[0m"
    else
        # Empty the contents from the stored cache, if any, and create the necessary directories
        rm -rf public/${COVERAGE_PATH}/*
        mkdir -p public/${COVERAGE_PATH} "public/${COVERAGE_PATH}/gcovr"

        # Generate the tracefile for the coverage reports
        lcov -q --capture --directory . -o coverage_tests
        lcov -q -a coverage_base -a coverage_tests -o coverage_total_unfiltered

        # Remove any unwanted files from coverage report, like external libraries
        lcov -q --remove coverage_total_unfiltered "${PWD}/lib/*" "${PWD}/CMakeFiles/*" "${PWD}/test/*" "${PWD}/src/main.cpp" -o coverage_total_filtered


        # Coverage generation using gcovr. Also generates the html page with the results
        # Output a summary (-s), sort by ascending percentage (-p), exclude files (-e)
        gcovr -s -p -e "${EXCLUDED_FILES}" --html --html-details --html-title "${HTML_TITLE}" -o public/${COVERAGE_PATH}/gcovr/gcovr.html
        gcovr -e "^.*(test|lib|main.cpp|CMakeFiles)"  # Generate coverage report for the CI

        # Render the html page for the lcov results
        genhtml --demangle-cpp -t "${PAGE_TITLE}" --html-epilog ci/page_style/epilog.html -o public/${COVERAGE_PATH} coverage_total_filtered
        cp ci/page_style/custom_format.css ci/page_style/epilog.html public/${COVERAGE_PATH}

        echo \
        "
        .title:after {
            content: \" for the ${CI_COMMIT_REF_NAME} branch\";
        }
        " >> public/${COVERAGE_PATH}/custom_format.css
    fi  # Test failure check
fi  # Build failure check


# Documentation generation
doxygen doxygen.conf
mv docs/html/* public/${DOCUMENT_PATH}

# Expired branch deletion
git branch -a | grep "remote" | xargs -n 1 -i sh -c "path=\"{}\"; basename \"\$path\"" > branches_list
ls -d public/coverage/*/ | xargs -n 1 -i sh -c "name=\"{}\"; basename \"\$name\"" > directory_list

# Condition the directory list (Remove unwanted instances)
sed -i -e '/gcovr/d;/inc/d;/src/d' directory_list

# Output for debugging purposes
echo -e "\e[1;36mBranch names list\e[0m"
cat branches_list

echo -e "\n\e[1;36mPublic directory contents list\e[0m"
cat directory_list


# Remove any expired branch folders
while read directory;
do
	if ! grep -q "^${directory}$" branches_list
	then
	    echo -e "\e[1;33m${directory} will be removed from the pages.\e[0m"
		rm -rf "public/${directory}" "public/coverage/${directory}" "public/docs/${directory}"
	fi
done < directory_list

echo -e "\e[1;92mDocumentation page for this branch:\e[0m \e[0;36m${CI_PAGES_URL}/${DOCUMENT_PATH}\e[0m"
echo -e "\e[1;92mCoverage Reports page for this branch:\e[0m \e[0;36m${CI_PAGES_URL}/${COVERAGE_PATH}\e[0m\n"
