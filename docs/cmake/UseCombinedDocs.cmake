include(FindPythonModule)
include(UseDoxygen)

#Find all required packages or Fail the Build
find_package(Doxygen REQUIRED)
find_package(PythonInterp REQUIRED)
find_package(Sphinx REQUIRED)
find_python_module(breathe REQUIRED)
find_python_module(exhale REQUIRED)

function(create_combined_doc)
    set(options)
    set(oneValueArgs
            SOURCE_DIR
            BUILD_DIR
            DOXY_CONF_FILE
            SPHINX_CONF_FILE
            SPHINX_INDEX_FILE
            SPHINX_THEME
            TARGET_NAME
            COMMENT
            )
    set(multiValueArgs)

    cmake_parse_arguments(DOC
            "${options}"
            "${oneValueArgs}"
            "${multiValueArgs}"
            ${ARGN}
            )
    #Create a Build Configuration For building Doxygen Docs
    create_doxygen_doc(
            BUILD_DIR
            ${DOC_BUILD_DIR}/doxygen
            CONF_FILE
            ${DOC_DOXY_CONF_FILE}
            TARGET_NAME
            docs
            COMMENT
            "Doxygen documentation"
    )

    #Parse the sphinx config file, replace with cmake args
    #and create a proper conf.py file for sphinx
    set(PARSED_SPHINX_CONF_FILE ${DOC_SOURCE_DIR}/conf.py)
    configure_file(${DOC_SPHINX_CONF_FILE} ${PARSED_SPHINX_CONF_FILE} @ONLY)

    #Check for Specified theme or else use the default
    if (NOT DEFINED DOC_SPHINX_THEME)
        set(SPHINX_THEME default)
    endif ()

    # Only regenerate Sphinx when:
    # - Doxygen rerun
    # - Our doc files have been updated
    # - The Sphinx config has been updated
    add_custom_command(OUTPUT ${DOC_SPHINX_INDEX_FILE}
            COMMAND
            ${SPHINX_EXECUTABLE} -b html
            # Tell Breathe where to find the Doxygen output
            -Dbreathe_projects.ecss-services=${DOC_BUILD_DIR}/doxygen/xml
            ${DOC_SOURCE_DIR} ${DOC_BUILD_DIR}/sphinx
            WORKING_DIRECTORY ${DOC_SOURCE_DIR}
            DEPENDS
            # Other docs files you want to track should go here (or in some variable)
            ${DOC_SPHINX_INDEX_FILE}
            ${DOXY_CONF_FILE}
            MAIN_DEPENDENCY ${PARSED_SPHINX_CONF_FILE}
            COMMENT "Generating documentation with Sphinx")

    # Nice named target so we can run the job easily
    add_custom_target(Generate_Docs ALL DEPENDS ${DOC_SPHINX_INDEX_FILE})

    # Add an install target to install the docs
    include(GNUInstallDirs)
    message(STATUS "Added ${DOC_TARGET_NAME} [Breathe+Sphinx+Doxygen+Exhale] target to build documentation")
endfunction()