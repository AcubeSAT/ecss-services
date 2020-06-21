find_package(Doxygen REQUIRED)

function(create_doxygen_doc)
    set(options)
    set(oneValueArgs
            BUILD_DIR
            CONF_FILE
            TARGET_NAME
            COMMENT)
    set(multiValueArgs)

    if (NOT DOXYGEN_FOUND)
        message(FATAL_ERROR "Doxygen is needed to build the documentation.")
    else ()
        cmake_parse_arguments(DOXY
                "${options}"
                "${oneValueArgs}"
                "${multiValueArgs}"
                ${ARGN}
                )
        #Doxygen won't create this directory for us
        file(MAKE_DIRECTORY ${DOXY_BUILD_DIR})

        #Replace variables inside @@ with the current values
        configure_file(${DOXY_CONF_FILE} ${DOCS_DIR}/Doxyfile @ONLY)

        #Starting from cmake 3.9 the usage of DOXYGEN_EXECUTABLE is deprecated
        if (TARGET Doxygen::doxygen)
            get_property(DOXYGEN_EXECUTABLE TARGET Doxygen::doxygen PROPERTY IMPORTED_LOCATION)
        endif ()

        add_custom_command(OUTPUT ${DOXY_BUILD_DIR}/xml/index.xml
                COMMAND Doxygen::doxygen ${DOCS_DIR}/Doxyfile
                MAIN_DEPENDENCY ${DOCS_DIR}/Doxyfile ${DOXY_CONF_FILE}
                WORKING_DIRECTORY ${DOXY_BUILD_DIR}
                COMMENT "Generating Doxygen documentation"
                VERBATIM)

        add_custom_target(Doxygen ALL DEPENDS ${DOCS_DIR}/Doxyfile)

        message(STATUS "Added ${DOXY_TARGET_NAME} target to build documentation")

    endif ()

endfunction()