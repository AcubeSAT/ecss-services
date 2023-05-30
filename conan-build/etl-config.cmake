########## MACROS ###########################################################################
#############################################################################################

# Requires CMake > 3.15
if(${CMAKE_VERSION} VERSION_LESS "3.15")
    message(FATAL_ERROR "The 'CMakeDeps' generator only works with CMake >= 3.15")
endif()

if(etl_FIND_QUIETLY)
    set(etl_MESSAGE_MODE VERBOSE)
else()
    set(etl_MESSAGE_MODE STATUS)
endif()

include(${CMAKE_CURRENT_LIST_DIR}/cmakedeps_macros.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/etlTargets.cmake)
include(CMakeFindDependencyMacro)

check_build_type_defined()

foreach(_DEPENDENCY ${etl_FIND_DEPENDENCY_NAMES} )
    # Check that we have not already called a find_package with the transitive dependency
    if(NOT ${_DEPENDENCY}_FOUND)
        find_dependency(${_DEPENDENCY} REQUIRED ${${_DEPENDENCY}_FIND_MODE})
    endif()
endforeach()

set(etl_VERSION_STRING "20.35.14")
set(etl_INCLUDE_DIRS ${etl_INCLUDE_DIRS_RELEASE} )
set(etl_INCLUDE_DIR ${etl_INCLUDE_DIRS_RELEASE} )
set(etl_LIBRARIES ${etl_LIBRARIES_RELEASE} )
set(etl_DEFINITIONS ${etl_DEFINITIONS_RELEASE} )

# Only the first installed configuration is included to avoid the collision
foreach(_BUILD_MODULE ${etl_BUILD_MODULES_PATHS_RELEASE} )
    message(${etl_MESSAGE_MODE} "Conan: Including build module from '${_BUILD_MODULE}'")
    include(${_BUILD_MODULE})
endforeach()


