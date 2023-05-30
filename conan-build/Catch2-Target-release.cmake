# Avoid multiple calls to find_package to append duplicated properties to the targets
include_guard()########### VARIABLES #######################################################################
#############################################################################################
set(catch2_FRAMEWORKS_FOUND_RELEASE "") # Will be filled later
conan_find_apple_frameworks(catch2_FRAMEWORKS_FOUND_RELEASE "${catch2_FRAMEWORKS_RELEASE}" "${catch2_FRAMEWORK_DIRS_RELEASE}")

set(catch2_LIBRARIES_TARGETS "") # Will be filled later


######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
if(NOT TARGET catch2_DEPS_TARGET)
    add_library(catch2_DEPS_TARGET INTERFACE IMPORTED)
endif()

set_property(TARGET catch2_DEPS_TARGET
             PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Release>:${catch2_FRAMEWORKS_FOUND_RELEASE}>
             $<$<CONFIG:Release>:${catch2_SYSTEM_LIBS_RELEASE}>
             $<$<CONFIG:Release>:Catch2::Catch2>
             APPEND)

####### Find the libraries declared in cpp_info.libs, create an IMPORTED target for each one and link the
####### catch2_DEPS_TARGET to all of them
conan_package_library_targets("${catch2_LIBS_RELEASE}"    # libraries
                              "${catch2_LIB_DIRS_RELEASE}" # package_libdir
                              "${catch2_BIN_DIRS_RELEASE}" # package_bindir
                              "${catch2_LIBRARY_TYPE_RELEASE}"
                              "${catch2_IS_HOST_WINDOWS_RELEASE}"
                              catch2_DEPS_TARGET
                              catch2_LIBRARIES_TARGETS  # out_libraries_targets
                              "_RELEASE"
                              "catch2"    # package_name
                              "${catch2_NO_SONAME_MODE_RELEASE}")  # soname

# FIXME: What is the result of this for multi-config? All configs adding themselves to path?
set(CMAKE_MODULE_PATH ${catch2_BUILD_DIRS_RELEASE} ${CMAKE_MODULE_PATH})

########## COMPONENTS TARGET PROPERTIES Release ########################################

    ########## COMPONENT Catch2::Catch2WithMain #############

        set(catch2_Catch2_Catch2WithMain_FRAMEWORKS_FOUND_RELEASE "")
        conan_find_apple_frameworks(catch2_Catch2_Catch2WithMain_FRAMEWORKS_FOUND_RELEASE "${catch2_Catch2_Catch2WithMain_FRAMEWORKS_RELEASE}" "${catch2_Catch2_Catch2WithMain_FRAMEWORK_DIRS_RELEASE}")

        set(catch2_Catch2_Catch2WithMain_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET catch2_Catch2_Catch2WithMain_DEPS_TARGET)
            add_library(catch2_Catch2_Catch2WithMain_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET catch2_Catch2_Catch2WithMain_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Release>:${catch2_Catch2_Catch2WithMain_FRAMEWORKS_FOUND_RELEASE}>
                     $<$<CONFIG:Release>:${catch2_Catch2_Catch2WithMain_SYSTEM_LIBS_RELEASE}>
                     $<$<CONFIG:Release>:${catch2_Catch2_Catch2WithMain_DEPENDENCIES_RELEASE}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'catch2_Catch2_Catch2WithMain_DEPS_TARGET' to all of them
        conan_package_library_targets("${catch2_Catch2_Catch2WithMain_LIBS_RELEASE}"
                              "${catch2_Catch2_Catch2WithMain_LIB_DIRS_RELEASE}"
                              "${catch2_Catch2_Catch2WithMain_BIN_DIRS_RELEASE}" # package_bindir
                              "${catch2_Catch2_Catch2WithMain_LIBRARY_TYPE_RELEASE}"
                              "${catch2_Catch2_Catch2WithMain_IS_HOST_WINDOWS_RELEASE}"
                              catch2_Catch2_Catch2WithMain_DEPS_TARGET
                              catch2_Catch2_Catch2WithMain_LIBRARIES_TARGETS
                              "_RELEASE"
                              "catch2_Catch2_Catch2WithMain"
                              "${catch2_Catch2_Catch2WithMain_NO_SONAME_MODE_RELEASE}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET Catch2::Catch2WithMain
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Release>:${catch2_Catch2_Catch2WithMain_OBJECTS_RELEASE}>
                     $<$<CONFIG:Release>:${catch2_Catch2_Catch2WithMain_LIBRARIES_TARGETS}>
                     APPEND)

        if("${catch2_Catch2_Catch2WithMain_LIBS_RELEASE}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET Catch2::Catch2WithMain
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         catch2_Catch2_Catch2WithMain_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET Catch2::Catch2WithMain PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Release>:${catch2_Catch2_Catch2WithMain_LINKER_FLAGS_RELEASE}> APPEND)
        set_property(TARGET Catch2::Catch2WithMain PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Release>:${catch2_Catch2_Catch2WithMain_INCLUDE_DIRS_RELEASE}> APPEND)
        set_property(TARGET Catch2::Catch2WithMain PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Release>:${catch2_Catch2_Catch2WithMain_LIB_DIRS_RELEASE}> APPEND)
        set_property(TARGET Catch2::Catch2WithMain PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Release>:${catch2_Catch2_Catch2WithMain_COMPILE_DEFINITIONS_RELEASE}> APPEND)
        set_property(TARGET Catch2::Catch2WithMain PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Release>:${catch2_Catch2_Catch2WithMain_COMPILE_OPTIONS_RELEASE}> APPEND)

    ########## COMPONENT Catch2::Catch2 #############

        set(catch2_Catch2_Catch2_FRAMEWORKS_FOUND_RELEASE "")
        conan_find_apple_frameworks(catch2_Catch2_Catch2_FRAMEWORKS_FOUND_RELEASE "${catch2_Catch2_Catch2_FRAMEWORKS_RELEASE}" "${catch2_Catch2_Catch2_FRAMEWORK_DIRS_RELEASE}")

        set(catch2_Catch2_Catch2_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET catch2_Catch2_Catch2_DEPS_TARGET)
            add_library(catch2_Catch2_Catch2_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET catch2_Catch2_Catch2_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Release>:${catch2_Catch2_Catch2_FRAMEWORKS_FOUND_RELEASE}>
                     $<$<CONFIG:Release>:${catch2_Catch2_Catch2_SYSTEM_LIBS_RELEASE}>
                     $<$<CONFIG:Release>:${catch2_Catch2_Catch2_DEPENDENCIES_RELEASE}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'catch2_Catch2_Catch2_DEPS_TARGET' to all of them
        conan_package_library_targets("${catch2_Catch2_Catch2_LIBS_RELEASE}"
                              "${catch2_Catch2_Catch2_LIB_DIRS_RELEASE}"
                              "${catch2_Catch2_Catch2_BIN_DIRS_RELEASE}" # package_bindir
                              "${catch2_Catch2_Catch2_LIBRARY_TYPE_RELEASE}"
                              "${catch2_Catch2_Catch2_IS_HOST_WINDOWS_RELEASE}"
                              catch2_Catch2_Catch2_DEPS_TARGET
                              catch2_Catch2_Catch2_LIBRARIES_TARGETS
                              "_RELEASE"
                              "catch2_Catch2_Catch2"
                              "${catch2_Catch2_Catch2_NO_SONAME_MODE_RELEASE}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET Catch2::Catch2
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Release>:${catch2_Catch2_Catch2_OBJECTS_RELEASE}>
                     $<$<CONFIG:Release>:${catch2_Catch2_Catch2_LIBRARIES_TARGETS}>
                     APPEND)

        if("${catch2_Catch2_Catch2_LIBS_RELEASE}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET Catch2::Catch2
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         catch2_Catch2_Catch2_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET Catch2::Catch2 PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Release>:${catch2_Catch2_Catch2_LINKER_FLAGS_RELEASE}> APPEND)
        set_property(TARGET Catch2::Catch2 PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Release>:${catch2_Catch2_Catch2_INCLUDE_DIRS_RELEASE}> APPEND)
        set_property(TARGET Catch2::Catch2 PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Release>:${catch2_Catch2_Catch2_LIB_DIRS_RELEASE}> APPEND)
        set_property(TARGET Catch2::Catch2 PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Release>:${catch2_Catch2_Catch2_COMPILE_DEFINITIONS_RELEASE}> APPEND)
        set_property(TARGET Catch2::Catch2 PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Release>:${catch2_Catch2_Catch2_COMPILE_OPTIONS_RELEASE}> APPEND)

    ########## AGGREGATED GLOBAL TARGET WITH THE COMPONENTS #####################
    set_property(TARGET Catch2::Catch2WithMain PROPERTY INTERFACE_LINK_LIBRARIES Catch2::Catch2WithMain APPEND)
    set_property(TARGET Catch2::Catch2WithMain PROPERTY INTERFACE_LINK_LIBRARIES Catch2::Catch2 APPEND)

########## For the modules (FindXXX)
set(catch2_LIBRARIES_RELEASE Catch2::Catch2WithMain)
