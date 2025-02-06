# Avoid multiple calls to find_package to append duplicated properties to the targets
include_guard()########### VARIABLES #######################################################################
#############################################################################################
set(logger_FRAMEWORKS_FOUND_RELEASE "") # Will be filled later
conan_find_apple_frameworks(logger_FRAMEWORKS_FOUND_RELEASE "${logger_FRAMEWORKS_RELEASE}" "${logger_FRAMEWORK_DIRS_RELEASE}")

set(logger_LIBRARIES_TARGETS "") # Will be filled later


######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
if(NOT TARGET logger_DEPS_TARGET)
    add_library(logger_DEPS_TARGET INTERFACE IMPORTED)
endif()

set_property(TARGET logger_DEPS_TARGET
             APPEND PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Release>:${logger_FRAMEWORKS_FOUND_RELEASE}>
             $<$<CONFIG:Release>:${logger_SYSTEM_LIBS_RELEASE}>
             $<$<CONFIG:Release>:etl::etl>)

####### Find the libraries declared in cpp_info.libs, create an IMPORTED target for each one and link the
####### logger_DEPS_TARGET to all of them
conan_package_library_targets("${logger_LIBS_RELEASE}"    # libraries
                              "${logger_LIB_DIRS_RELEASE}" # package_libdir
                              "${logger_BIN_DIRS_RELEASE}" # package_bindir
                              "${logger_LIBRARY_TYPE_RELEASE}"
                              "${logger_IS_HOST_WINDOWS_RELEASE}"
                              logger_DEPS_TARGET
                              logger_LIBRARIES_TARGETS  # out_libraries_targets
                              "_RELEASE"
                              "logger"    # package_name
                              "${logger_NO_SONAME_MODE_RELEASE}")  # soname

# FIXME: What is the result of this for multi-config? All configs adding themselves to path?
set(CMAKE_MODULE_PATH ${logger_BUILD_DIRS_RELEASE} ${CMAKE_MODULE_PATH})

########## COMPONENTS TARGET PROPERTIES Release ########################################

    ########## COMPONENT log_x86 #############

        set(logger_log_x86_FRAMEWORKS_FOUND_RELEASE "")
        conan_find_apple_frameworks(logger_log_x86_FRAMEWORKS_FOUND_RELEASE "${logger_log_x86_FRAMEWORKS_RELEASE}" "${logger_log_x86_FRAMEWORK_DIRS_RELEASE}")

        set(logger_log_x86_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET logger_log_x86_DEPS_TARGET)
            add_library(logger_log_x86_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET logger_log_x86_DEPS_TARGET
                     APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Release>:${logger_log_x86_FRAMEWORKS_FOUND_RELEASE}>
                     $<$<CONFIG:Release>:${logger_log_x86_SYSTEM_LIBS_RELEASE}>
                     $<$<CONFIG:Release>:${logger_log_x86_DEPENDENCIES_RELEASE}>
                     )

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'logger_log_x86_DEPS_TARGET' to all of them
        conan_package_library_targets("${logger_log_x86_LIBS_RELEASE}"
                              "${logger_log_x86_LIB_DIRS_RELEASE}"
                              "${logger_log_x86_BIN_DIRS_RELEASE}" # package_bindir
                              "${logger_log_x86_LIBRARY_TYPE_RELEASE}"
                              "${logger_log_x86_IS_HOST_WINDOWS_RELEASE}"
                              logger_log_x86_DEPS_TARGET
                              logger_log_x86_LIBRARIES_TARGETS
                              "_RELEASE"
                              "logger_log_x86"
                              "${logger_log_x86_NO_SONAME_MODE_RELEASE}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET log_x86
                     APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Release>:${logger_log_x86_OBJECTS_RELEASE}>
                     $<$<CONFIG:Release>:${logger_log_x86_LIBRARIES_TARGETS}>
                     )

        if("${logger_log_x86_LIBS_RELEASE}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET log_x86
                         APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                         logger_log_x86_DEPS_TARGET)
        endif()

        set_property(TARGET log_x86 APPEND PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Release>:${logger_log_x86_LINKER_FLAGS_RELEASE}>)
        set_property(TARGET log_x86 APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Release>:${logger_log_x86_INCLUDE_DIRS_RELEASE}>)
        set_property(TARGET log_x86 APPEND PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Release>:${logger_log_x86_LIB_DIRS_RELEASE}>)
        set_property(TARGET log_x86 APPEND PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Release>:${logger_log_x86_COMPILE_DEFINITIONS_RELEASE}>)
        set_property(TARGET log_x86 APPEND PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Release>:${logger_log_x86_COMPILE_OPTIONS_RELEASE}>)

    ########## COMPONENT log_common #############

        set(logger_log_common_FRAMEWORKS_FOUND_RELEASE "")
        conan_find_apple_frameworks(logger_log_common_FRAMEWORKS_FOUND_RELEASE "${logger_log_common_FRAMEWORKS_RELEASE}" "${logger_log_common_FRAMEWORK_DIRS_RELEASE}")

        set(logger_log_common_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET logger_log_common_DEPS_TARGET)
            add_library(logger_log_common_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET logger_log_common_DEPS_TARGET
                     APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Release>:${logger_log_common_FRAMEWORKS_FOUND_RELEASE}>
                     $<$<CONFIG:Release>:${logger_log_common_SYSTEM_LIBS_RELEASE}>
                     $<$<CONFIG:Release>:${logger_log_common_DEPENDENCIES_RELEASE}>
                     )

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'logger_log_common_DEPS_TARGET' to all of them
        conan_package_library_targets("${logger_log_common_LIBS_RELEASE}"
                              "${logger_log_common_LIB_DIRS_RELEASE}"
                              "${logger_log_common_BIN_DIRS_RELEASE}" # package_bindir
                              "${logger_log_common_LIBRARY_TYPE_RELEASE}"
                              "${logger_log_common_IS_HOST_WINDOWS_RELEASE}"
                              logger_log_common_DEPS_TARGET
                              logger_log_common_LIBRARIES_TARGETS
                              "_RELEASE"
                              "logger_log_common"
                              "${logger_log_common_NO_SONAME_MODE_RELEASE}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET log_common
                     APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Release>:${logger_log_common_OBJECTS_RELEASE}>
                     $<$<CONFIG:Release>:${logger_log_common_LIBRARIES_TARGETS}>
                     )

        if("${logger_log_common_LIBS_RELEASE}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET log_common
                         APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                         logger_log_common_DEPS_TARGET)
        endif()

        set_property(TARGET log_common APPEND PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Release>:${logger_log_common_LINKER_FLAGS_RELEASE}>)
        set_property(TARGET log_common APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Release>:${logger_log_common_INCLUDE_DIRS_RELEASE}>)
        set_property(TARGET log_common APPEND PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Release>:${logger_log_common_LIB_DIRS_RELEASE}>)
        set_property(TARGET log_common APPEND PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Release>:${logger_log_common_COMPILE_DEFINITIONS_RELEASE}>)
        set_property(TARGET log_common APPEND PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Release>:${logger_log_common_COMPILE_OPTIONS_RELEASE}>)

    ########## AGGREGATED GLOBAL TARGET WITH THE COMPONENTS #####################
    set_property(TARGET logger::logger APPEND PROPERTY INTERFACE_LINK_LIBRARIES log_x86)
    set_property(TARGET logger::logger APPEND PROPERTY INTERFACE_LINK_LIBRARIES log_common)

########## For the modules (FindXXX)
set(logger_LIBRARIES_RELEASE logger::logger)
