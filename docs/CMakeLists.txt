find_package(Doxygen)
option(BUILD_DOCUMENTATION "Create and install the HTML based API
documentation (requires Doxygen)" ${DOXYGEN_FOUND})

if(BUILD_DOCUMENTATION)
    if(NOT DOXYGEN_FOUND)
        message(FATAL_ERROR "Doxygen is needed to build the documentation.")
    endif()

    set(DOXYGEN_INPUT_DIR ${PROJECT_SOURCE_DIR})
    set(DOCS_DIR ${PROJECT_SOURCE_DIR}/docs)
    set(DOXYGEN_OUTPUT_DIR ${DOCS_DIR}/output/doxygen)
    set(DOXYGEN_INDEX_FILE ${DOXYGEN_OUTPUT_DIR}/html/index.html)
    set(DOXYFILE_IN ${DOCS_DIR}/doxygen.conf)
    set(DOXYFILE_OUT ${DOCS_DIR}/output/MakeDoxyConf)

    #Replace variables inside @@ with the current values
    configure_file(${DOXYFILE_IN} ${DOXYFILE_OUT} @ONLY)

    file(MAKE_DIRECTORY ${DOXYGEN_OUTPUT_DIR}) #Doxygen won't create this for us

    #starting from cmake 3.9 the usage of DOXYGEN_EXECUTABLE is deprecated
    if(TARGET Doxygen::doxygen)
        get_property(DOXYGEN_EXECUTABLE TARGET Doxygen::doxygen PROPERTY IMPORTED_LOCATION)
    endif()

    add_custom_command(OUTPUT ${DOXYGEN_INDEX_FILE}
            COMMAND Doxygen::doxygen ${DOXYFILE_OUT}
            MAIN_DEPENDENCY ${DOXYFILE_OUT} ${DOXYFILE_IN}
            WORKING_DIRECTORY ${DOCS_DIR}
            COMMENT "Generating docs"
            VERBATIM)

    add_custom_target(Doxygen ALL DEPENDS ${DOXYGEN_INDEX_FILE})
endif()

find_package(Sphinx REQUIRED)
set(SPHINX_SOURCE ${DOCS_DIR})
set(SPHINX_BUILD ${DOCS_DIR}/output/sphinx)
set(SPHINX_INDEX_FILE ${SPHINX_BUILD}/index.html)

# Only regenerate Sphinx when:
# - Doxygen rerun
# - Our doc files have been updated
# - The Sphinx config has been updated
add_custom_command(OUTPUT ${SPHINX_INDEX_FILE}
        COMMAND
        ${SPHINX_EXECUTABLE} -b html
        # Tell Breathe where to find the Doxygen output
        -Dbreathe_projects.ecss-services=${DOXYGEN_OUTPUT_DIR}/xml
        ${SPHINX_SOURCE} ${SPHINX_BUILD}
        WORKING_DIRECTORY ${DOCS_DIR}
        DEPENDS
        # Other docs files you want to track should go here (or in some variable)
        ${DOCS_DIR}/index.rst
        ${DOXYGEN_INDEX_FILE}
        MAIN_DEPENDENCY ${SPHINX_SOURCE}/conf.py
        COMMENT "Generating documentation with Sphinx")

# Nice named target so we can run the job easily
add_custom_target(Sphinx ALL DEPENDS ${SPHINX_INDEX_FILE})

# Add an install target to install the docs
include(GNUInstallDirs)
install(DIRECTORY ${SPHINX_BUILD}
        DESTINATION ${CMAKE_INSTALL_DOCDIR})