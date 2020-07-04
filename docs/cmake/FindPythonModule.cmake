include(FindPackageHandleStandardArgs)

#Searches the OS for a  specified python module
function(find_python_module module)
    # Build Fail if Python interpreter was not found
    if (NOT PYTHON_EXECUTABLE)
        message(FATAL_ERROR "Use find_package(PythonInterp) first!")
    endif ()
    string(TOLOWER ${module} _module_lower)
    if (NOT ${_module_lower})
        if (ARGC GREATER 1 AND ARGV1 STREQUAL "REQUIRED")
            set(${module}_FIND_REQUIRED TRUE)
        endif ()
        # Find module location
        execute_process(
                COMMAND
                ${PYTHON_EXECUTABLE} "-c" "import re, ${_module_lower}; print(re.compile('/__init__.py.*').sub('',${_module_lower}.__file__))"
                RESULT_VARIABLE _${module}_status
                OUTPUT_VARIABLE _${module}_location
                ERROR_QUIET
                OUTPUT_STRIP_TRAILING_WHITESPACE
        )
        if (NOT _${module}_status)
            set(${module} ${_${module}_location} CACHE STRING "Location of Python module ${module}")
        endif ()
        # Find module version
        execute_process(
                COMMAND
                ${PYTHON_EXECUTABLE} "-c" "import re, ${_module_lower}; print(re.compile('/__init__.py.*').sub('',${_module_lower}.__version__))"
                OUTPUT_VARIABLE _${module}_ver
                ERROR_QUIET
                OUTPUT_STRIP_TRAILING_WHITESPACE
        )
    endif ()

    find_package_handle_standard_args(${module}
            FOUND_VAR ${module}_FOUND
            REQUIRED_VARS ${module}
            VERSION_VAR _${module}_ver
            )
endfunction()