# Avoid multiple calls to find_package to append duplicated properties to the targets
include_guard()########### VARIABLES #######################################################################
#############################################################################################
set(etl_FRAMEWORKS_FOUND_RELEASE "") # Will be filled later
conan_find_apple_frameworks(etl_FRAMEWORKS_FOUND_RELEASE "${etl_FRAMEWORKS_RELEASE}" "${etl_FRAMEWORK_DIRS_RELEASE}")

set(etl_LIBRARIES_TARGETS "") # Will be filled later


######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
if(NOT TARGET etl_DEPS_TARGET)
    add_library(etl_DEPS_TARGET INTERFACE IMPORTED)
endif()

set_property(TARGET etl_DEPS_TARGET
             PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Release>:${etl_FRAMEWORKS_FOUND_RELEASE}>
             $<$<CONFIG:Release>:${etl_SYSTEM_LIBS_RELEASE}>
             $<$<CONFIG:Release>:>
             APPEND)

####### Find the libraries declared in cpp_info.libs, create an IMPORTED target for each one and link the
####### etl_DEPS_TARGET to all of them
conan_package_library_targets("${etl_LIBS_RELEASE}"    # libraries
                              "${etl_LIB_DIRS_RELEASE}" # package_libdir
                              "${etl_BIN_DIRS_RELEASE}" # package_bindir
                              "${etl_LIBRARY_TYPE_RELEASE}"
                              "${etl_IS_HOST_WINDOWS_RELEASE}"
                              etl_DEPS_TARGET
                              etl_LIBRARIES_TARGETS  # out_libraries_targets
                              "_RELEASE"
                              "etl"    # package_name
                              "${etl_NO_SONAME_MODE_RELEASE}")  # soname

# FIXME: What is the result of this for multi-config? All configs adding themselves to path?
set(CMAKE_MODULE_PATH ${etl_BUILD_DIRS_RELEASE} ${CMAKE_MODULE_PATH})

########## GLOBAL TARGET PROPERTIES Release ########################################
    set_property(TARGET etl
                 PROPERTY INTERFACE_LINK_LIBRARIES
                 $<$<CONFIG:Release>:${etl_OBJECTS_RELEASE}>
                 $<$<CONFIG:Release>:${etl_LIBRARIES_TARGETS}>
                 APPEND)

    if("${etl_LIBS_RELEASE}" STREQUAL "")
        # If the package is not declaring any "cpp_info.libs" the package deps, system libs,
        # frameworks etc are not linked to the imported targets and we need to do it to the
        # global target
        set_property(TARGET etl
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     etl_DEPS_TARGET
                     APPEND)
    endif()

    set_property(TARGET etl
                 PROPERTY INTERFACE_LINK_OPTIONS
                 $<$<CONFIG:Release>:${etl_LINKER_FLAGS_RELEASE}> APPEND)
    set_property(TARGET etl
                 PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                 $<$<CONFIG:Release>:${etl_INCLUDE_DIRS_RELEASE}> APPEND)
    # Necessary to find LINK shared libraries in Linux
    set_property(TARGET etl
                 PROPERTY INTERFACE_LINK_DIRECTORIES
                 $<$<CONFIG:Release>:${etl_LIB_DIRS_RELEASE}> APPEND)
    set_property(TARGET etl
                 PROPERTY INTERFACE_COMPILE_DEFINITIONS
                 $<$<CONFIG:Release>:${etl_COMPILE_DEFINITIONS_RELEASE}> APPEND)
    set_property(TARGET etl
                 PROPERTY INTERFACE_COMPILE_OPTIONS
                 $<$<CONFIG:Release>:${etl_COMPILE_OPTIONS_RELEASE}> APPEND)

########## For the modules (FindXXX)
set(etl_LIBRARIES_RELEASE etl)
