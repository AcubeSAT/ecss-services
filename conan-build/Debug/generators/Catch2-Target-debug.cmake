# Avoid multiple calls to find_package to append duplicated properties to the targets
include_guard()########### VARIABLES #######################################################################
#############################################################################################
set(catch2_FRAMEWORKS_FOUND_DEBUG "") # Will be filled later
conan_find_apple_frameworks(catch2_FRAMEWORKS_FOUND_DEBUG "${catch2_FRAMEWORKS_DEBUG}" "${catch2_FRAMEWORK_DIRS_DEBUG}")

set(catch2_LIBRARIES_TARGETS "") # Will be filled later


######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
if(NOT TARGET catch2_DEPS_TARGET)
    add_library(catch2_DEPS_TARGET INTERFACE IMPORTED)
endif()

set_property(TARGET catch2_DEPS_TARGET
             APPEND PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Debug>:${catch2_FRAMEWORKS_FOUND_DEBUG}>
             $<$<CONFIG:Debug>:${catch2_SYSTEM_LIBS_DEBUG}>
             $<$<CONFIG:Debug>:Catch2::Catch2>)

####### Find the libraries declared in cpp_info.libs, create an IMPORTED target for each one and link the
####### catch2_DEPS_TARGET to all of them
conan_package_library_targets("${catch2_LIBS_DEBUG}"    # libraries
                              "${catch2_LIB_DIRS_DEBUG}" # package_libdir
                              "${catch2_BIN_DIRS_DEBUG}" # package_bindir
                              "${catch2_LIBRARY_TYPE_DEBUG}"
                              "${catch2_IS_HOST_WINDOWS_DEBUG}"
                              catch2_DEPS_TARGET
                              catch2_LIBRARIES_TARGETS  # out_libraries_targets
                              "_DEBUG"
                              "catch2"    # package_name
                              "${catch2_NO_SONAME_MODE_DEBUG}")  # soname

# FIXME: What is the result of this for multi-config? All configs adding themselves to path?
set(CMAKE_MODULE_PATH ${catch2_BUILD_DIRS_DEBUG} ${CMAKE_MODULE_PATH})

########## COMPONENTS TARGET PROPERTIES Debug ########################################

    ########## COMPONENT Catch2::Catch2WithMain #############

        set(catch2_Catch2_Catch2WithMain_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(catch2_Catch2_Catch2WithMain_FRAMEWORKS_FOUND_DEBUG "${catch2_Catch2_Catch2WithMain_FRAMEWORKS_DEBUG}" "${catch2_Catch2_Catch2WithMain_FRAMEWORK_DIRS_DEBUG}")

        set(catch2_Catch2_Catch2WithMain_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET catch2_Catch2_Catch2WithMain_DEPS_TARGET)
            add_library(catch2_Catch2_Catch2WithMain_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET catch2_Catch2_Catch2WithMain_DEPS_TARGET
                     APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${catch2_Catch2_Catch2WithMain_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${catch2_Catch2_Catch2WithMain_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${catch2_Catch2_Catch2WithMain_DEPENDENCIES_DEBUG}>
                     )

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'catch2_Catch2_Catch2WithMain_DEPS_TARGET' to all of them
        conan_package_library_targets("${catch2_Catch2_Catch2WithMain_LIBS_DEBUG}"
                              "${catch2_Catch2_Catch2WithMain_LIB_DIRS_DEBUG}"
                              "${catch2_Catch2_Catch2WithMain_BIN_DIRS_DEBUG}" # package_bindir
                              "${catch2_Catch2_Catch2WithMain_LIBRARY_TYPE_DEBUG}"
                              "${catch2_Catch2_Catch2WithMain_IS_HOST_WINDOWS_DEBUG}"
                              catch2_Catch2_Catch2WithMain_DEPS_TARGET
                              catch2_Catch2_Catch2WithMain_LIBRARIES_TARGETS
                              "_DEBUG"
                              "catch2_Catch2_Catch2WithMain"
                              "${catch2_Catch2_Catch2WithMain_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET Catch2::Catch2WithMain
                     APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${catch2_Catch2_Catch2WithMain_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${catch2_Catch2_Catch2WithMain_LIBRARIES_TARGETS}>
                     )

        if("${catch2_Catch2_Catch2WithMain_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET Catch2::Catch2WithMain
                         APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                         catch2_Catch2_Catch2WithMain_DEPS_TARGET)
        endif()

        set_property(TARGET Catch2::Catch2WithMain APPEND PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${catch2_Catch2_Catch2WithMain_LINKER_FLAGS_DEBUG}>)
        set_property(TARGET Catch2::Catch2WithMain APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${catch2_Catch2_Catch2WithMain_INCLUDE_DIRS_DEBUG}>)
        set_property(TARGET Catch2::Catch2WithMain APPEND PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${catch2_Catch2_Catch2WithMain_LIB_DIRS_DEBUG}>)
        set_property(TARGET Catch2::Catch2WithMain APPEND PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${catch2_Catch2_Catch2WithMain_COMPILE_DEFINITIONS_DEBUG}>)
        set_property(TARGET Catch2::Catch2WithMain APPEND PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${catch2_Catch2_Catch2WithMain_COMPILE_OPTIONS_DEBUG}>)


    ########## COMPONENT Catch2::Catch2 #############

        set(catch2_Catch2_Catch2_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(catch2_Catch2_Catch2_FRAMEWORKS_FOUND_DEBUG "${catch2_Catch2_Catch2_FRAMEWORKS_DEBUG}" "${catch2_Catch2_Catch2_FRAMEWORK_DIRS_DEBUG}")

        set(catch2_Catch2_Catch2_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET catch2_Catch2_Catch2_DEPS_TARGET)
            add_library(catch2_Catch2_Catch2_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET catch2_Catch2_Catch2_DEPS_TARGET
                     APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${catch2_Catch2_Catch2_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${catch2_Catch2_Catch2_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${catch2_Catch2_Catch2_DEPENDENCIES_DEBUG}>
                     )

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'catch2_Catch2_Catch2_DEPS_TARGET' to all of them
        conan_package_library_targets("${catch2_Catch2_Catch2_LIBS_DEBUG}"
                              "${catch2_Catch2_Catch2_LIB_DIRS_DEBUG}"
                              "${catch2_Catch2_Catch2_BIN_DIRS_DEBUG}" # package_bindir
                              "${catch2_Catch2_Catch2_LIBRARY_TYPE_DEBUG}"
                              "${catch2_Catch2_Catch2_IS_HOST_WINDOWS_DEBUG}"
                              catch2_Catch2_Catch2_DEPS_TARGET
                              catch2_Catch2_Catch2_LIBRARIES_TARGETS
                              "_DEBUG"
                              "catch2_Catch2_Catch2"
                              "${catch2_Catch2_Catch2_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET Catch2::Catch2
                     APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${catch2_Catch2_Catch2_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${catch2_Catch2_Catch2_LIBRARIES_TARGETS}>
                     )

        if("${catch2_Catch2_Catch2_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET Catch2::Catch2
                         APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                         catch2_Catch2_Catch2_DEPS_TARGET)
        endif()

        set_property(TARGET Catch2::Catch2 APPEND PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${catch2_Catch2_Catch2_LINKER_FLAGS_DEBUG}>)
        set_property(TARGET Catch2::Catch2 APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${catch2_Catch2_Catch2_INCLUDE_DIRS_DEBUG}>)
        set_property(TARGET Catch2::Catch2 APPEND PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${catch2_Catch2_Catch2_LIB_DIRS_DEBUG}>)
        set_property(TARGET Catch2::Catch2 APPEND PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${catch2_Catch2_Catch2_COMPILE_DEFINITIONS_DEBUG}>)
        set_property(TARGET Catch2::Catch2 APPEND PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${catch2_Catch2_Catch2_COMPILE_OPTIONS_DEBUG}>)


    ########## AGGREGATED GLOBAL TARGET WITH THE COMPONENTS #####################
    set_property(TARGET Catch2::Catch2WithMain APPEND PROPERTY INTERFACE_LINK_LIBRARIES Catch2::Catch2WithMain)
    set_property(TARGET Catch2::Catch2WithMain APPEND PROPERTY INTERFACE_LINK_LIBRARIES Catch2::Catch2)

########## For the modules (FindXXX)
set(catch2_LIBRARIES_DEBUG Catch2::Catch2WithMain)
