########## MACROS ###########################################################################
#############################################################################################

# Requires CMake > 3.15
if(${CMAKE_VERSION} VERSION_LESS "3.15")
    message(FATAL_ERROR "The 'CMakeDeps' generator only works with CMake >= 3.15")
endif()

if(Catch2_FIND_QUIETLY)
    set(Catch2_MESSAGE_MODE VERBOSE)
else()
    set(Catch2_MESSAGE_MODE STATUS)
endif()

include(${CMAKE_CURRENT_LIST_DIR}/cmakedeps_macros.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/Catch2Targets.cmake)
include(CMakeFindDependencyMacro)

check_build_type_defined()

foreach(_DEPENDENCY ${catch2_FIND_DEPENDENCY_NAMES} )
    # Check that we have not already called a find_package with the transitive dependency
    if(NOT ${_DEPENDENCY}_FOUND)
        find_dependency(${_DEPENDENCY} REQUIRED ${${_DEPENDENCY}_FIND_MODE})
    endif()
endforeach()

set(Catch2_VERSION_STRING "3.3.1")
set(Catch2_INCLUDE_DIRS ${catch2_INCLUDE_DIRS_RELEASE} )
set(Catch2_INCLUDE_DIR ${catch2_INCLUDE_DIRS_RELEASE} )
set(Catch2_LIBRARIES ${catch2_LIBRARIES_RELEASE} )
set(Catch2_DEFINITIONS ${catch2_DEFINITIONS_RELEASE} )

# Only the first installed configuration is included to avoid the collision
foreach(_BUILD_MODULE ${catch2_BUILD_MODULES_PATHS_RELEASE} )
    message(${Catch2_MESSAGE_MODE} "Conan: Including build module from '${_BUILD_MODULE}'")
    include(${_BUILD_MODULE})
endforeach()


