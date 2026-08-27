# Avoid multiple calls to find_package to append duplicated properties to the targets
include_guard()########### VARIABLES #######################################################################
#############################################################################################
set(logger_FRAMEWORKS_FOUND_DEBUG "") # Will be filled later
conan_find_apple_frameworks(logger_FRAMEWORKS_FOUND_DEBUG "${logger_FRAMEWORKS_DEBUG}" "${logger_FRAMEWORK_DIRS_DEBUG}")

set(logger_LIBRARIES_TARGETS "") # Will be filled later


######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
if(NOT TARGET logger_DEPS_TARGET)
    add_library(logger_DEPS_TARGET INTERFACE IMPORTED)
endif()

set_property(TARGET logger_DEPS_TARGET
             APPEND PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Debug>:${logger_FRAMEWORKS_FOUND_DEBUG}>
             $<$<CONFIG:Debug>:${logger_SYSTEM_LIBS_DEBUG}>
             $<$<CONFIG:Debug>:etl::etl>)

####### Find the libraries declared in cpp_info.libs, create an IMPORTED target for each one and link the
####### logger_DEPS_TARGET to all of them
conan_package_library_targets("${logger_LIBS_DEBUG}"    # libraries
                              "${logger_LIB_DIRS_DEBUG}" # package_libdir
                              "${logger_BIN_DIRS_DEBUG}" # package_bindir
                              "${logger_LIBRARY_TYPE_DEBUG}"
                              "${logger_IS_HOST_WINDOWS_DEBUG}"
                              logger_DEPS_TARGET
                              logger_LIBRARIES_TARGETS  # out_libraries_targets
                              "_DEBUG"
                              "logger"    # package_name
                              "${logger_NO_SONAME_MODE_DEBUG}")  # soname

# FIXME: What is the result of this for multi-config? All configs adding themselves to path?
set(CMAKE_MODULE_PATH ${logger_BUILD_DIRS_DEBUG} ${CMAKE_MODULE_PATH})

########## COMPONENTS TARGET PROPERTIES Debug ########################################

    ########## COMPONENT log_x86 #############

        set(logger_log_x86_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(logger_log_x86_FRAMEWORKS_FOUND_DEBUG "${logger_log_x86_FRAMEWORKS_DEBUG}" "${logger_log_x86_FRAMEWORK_DIRS_DEBUG}")

        set(logger_log_x86_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET logger_log_x86_DEPS_TARGET)
            add_library(logger_log_x86_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET logger_log_x86_DEPS_TARGET
                     APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${logger_log_x86_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${logger_log_x86_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${logger_log_x86_DEPENDENCIES_DEBUG}>
                     )

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'logger_log_x86_DEPS_TARGET' to all of them
        conan_package_library_targets("${logger_log_x86_LIBS_DEBUG}"
                              "${logger_log_x86_LIB_DIRS_DEBUG}"
                              "${logger_log_x86_BIN_DIRS_DEBUG}" # package_bindir
                              "${logger_log_x86_LIBRARY_TYPE_DEBUG}"
                              "${logger_log_x86_IS_HOST_WINDOWS_DEBUG}"
                              logger_log_x86_DEPS_TARGET
                              logger_log_x86_LIBRARIES_TARGETS
                              "_DEBUG"
                              "logger_log_x86"
                              "${logger_log_x86_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET log_x86
                     APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${logger_log_x86_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${logger_log_x86_LIBRARIES_TARGETS}>
                     )

        if("${logger_log_x86_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET log_x86
                         APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                         logger_log_x86_DEPS_TARGET)
        endif()

        set_property(TARGET log_x86 APPEND PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${logger_log_x86_LINKER_FLAGS_DEBUG}>)
        set_property(TARGET log_x86 APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${logger_log_x86_INCLUDE_DIRS_DEBUG}>)
        set_property(TARGET log_x86 APPEND PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${logger_log_x86_LIB_DIRS_DEBUG}>)
        set_property(TARGET log_x86 APPEND PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${logger_log_x86_COMPILE_DEFINITIONS_DEBUG}>)
        set_property(TARGET log_x86 APPEND PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${logger_log_x86_COMPILE_OPTIONS_DEBUG}>)


    ########## COMPONENT log_common #############

        set(logger_log_common_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(logger_log_common_FRAMEWORKS_FOUND_DEBUG "${logger_log_common_FRAMEWORKS_DEBUG}" "${logger_log_common_FRAMEWORK_DIRS_DEBUG}")

        set(logger_log_common_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET logger_log_common_DEPS_TARGET)
            add_library(logger_log_common_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET logger_log_common_DEPS_TARGET
                     APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${logger_log_common_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${logger_log_common_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${logger_log_common_DEPENDENCIES_DEBUG}>
                     )

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'logger_log_common_DEPS_TARGET' to all of them
        conan_package_library_targets("${logger_log_common_LIBS_DEBUG}"
                              "${logger_log_common_LIB_DIRS_DEBUG}"
                              "${logger_log_common_BIN_DIRS_DEBUG}" # package_bindir
                              "${logger_log_common_LIBRARY_TYPE_DEBUG}"
                              "${logger_log_common_IS_HOST_WINDOWS_DEBUG}"
                              logger_log_common_DEPS_TARGET
                              logger_log_common_LIBRARIES_TARGETS
                              "_DEBUG"
                              "logger_log_common"
                              "${logger_log_common_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET log_common
                     APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${logger_log_common_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${logger_log_common_LIBRARIES_TARGETS}>
                     )

        if("${logger_log_common_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET log_common
                         APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                         logger_log_common_DEPS_TARGET)
        endif()

        set_property(TARGET log_common APPEND PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${logger_log_common_LINKER_FLAGS_DEBUG}>)
        set_property(TARGET log_common APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${logger_log_common_INCLUDE_DIRS_DEBUG}>)
        set_property(TARGET log_common APPEND PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${logger_log_common_LIB_DIRS_DEBUG}>)
        set_property(TARGET log_common APPEND PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${logger_log_common_COMPILE_DEFINITIONS_DEBUG}>)
        set_property(TARGET log_common APPEND PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${logger_log_common_COMPILE_OPTIONS_DEBUG}>)


    ########## AGGREGATED GLOBAL TARGET WITH THE COMPONENTS #####################
    set_property(TARGET logger::logger APPEND PROPERTY INTERFACE_LINK_LIBRARIES log_x86)
    set_property(TARGET logger::logger APPEND PROPERTY INTERFACE_LINK_LIBRARIES log_common)

########## For the modules (FindXXX)
set(logger_LIBRARIES_DEBUG logger::logger)
