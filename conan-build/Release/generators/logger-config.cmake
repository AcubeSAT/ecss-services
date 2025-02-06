########## MACROS ###########################################################################
#############################################################################################

# Requires CMake > 3.15
if(${CMAKE_VERSION} VERSION_LESS "3.15")
    message(FATAL_ERROR "The 'CMakeDeps' generator only works with CMake >= 3.15")
endif()

if(logger_FIND_QUIETLY)
    set(logger_MESSAGE_MODE VERBOSE)
else()
    set(logger_MESSAGE_MODE STATUS)
endif()

include(${CMAKE_CURRENT_LIST_DIR}/cmakedeps_macros.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/loggerTargets.cmake)
include(CMakeFindDependencyMacro)

check_build_type_defined()

foreach(_DEPENDENCY ${logger_FIND_DEPENDENCY_NAMES} )
    # Check that we have not already called a find_package with the transitive dependency
    if(NOT ${_DEPENDENCY}_FOUND)
        find_dependency(${_DEPENDENCY} REQUIRED ${${_DEPENDENCY}_FIND_MODE})
    endif()
endforeach()

set(logger_VERSION_STRING "1.0")
set(logger_INCLUDE_DIRS ${logger_INCLUDE_DIRS_RELEASE} )
set(logger_INCLUDE_DIR ${logger_INCLUDE_DIRS_RELEASE} )
set(logger_LIBRARIES ${logger_LIBRARIES_RELEASE} )
set(logger_DEFINITIONS ${logger_DEFINITIONS_RELEASE} )

# Only the first installed configuration is included to avoid the collision
foreach(_BUILD_MODULE ${logger_BUILD_MODULES_PATHS_RELEASE} )
    message(${logger_MESSAGE_MODE} "Conan: Including build module from '${_BUILD_MODULE}'")
    include(${_BUILD_MODULE})
endforeach()


