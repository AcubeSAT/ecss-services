########### AGGREGATED COMPONENTS AND DEPENDENCIES FOR THE MULTI CONFIG #####################
#############################################################################################

list(APPEND catch2_COMPONENT_NAMES Catch2::Catch2 Catch2::Catch2WithMain)
list(REMOVE_DUPLICATES catch2_COMPONENT_NAMES)
set(catch2_FIND_DEPENDENCY_NAMES "")

########### VARIABLES #######################################################################
#############################################################################################
set(catch2_PACKAGE_FOLDER_RELEASE "/home/athanasios/.conan2/p/b/catch0e2a80c12239b/p")
set(catch2_BUILD_MODULES_PATHS_RELEASE )


set(catch2_INCLUDE_DIRS_RELEASE "${catch2_PACKAGE_FOLDER_RELEASE}/include")
set(catch2_RES_DIRS_RELEASE )
set(catch2_DEFINITIONS_RELEASE )
set(catch2_SHARED_LINK_FLAGS_RELEASE )
set(catch2_EXE_LINK_FLAGS_RELEASE )
set(catch2_OBJECTS_RELEASE )
set(catch2_COMPILE_DEFINITIONS_RELEASE )
set(catch2_COMPILE_OPTIONS_C_RELEASE )
set(catch2_COMPILE_OPTIONS_CXX_RELEASE )
set(catch2_LIB_DIRS_RELEASE "${catch2_PACKAGE_FOLDER_RELEASE}/lib")
set(catch2_BIN_DIRS_RELEASE )
set(catch2_LIBRARY_TYPE_RELEASE STATIC)
set(catch2_IS_HOST_WINDOWS_RELEASE 0)
set(catch2_LIBS_RELEASE Catch2Main Catch2)
set(catch2_SYSTEM_LIBS_RELEASE m)
set(catch2_FRAMEWORK_DIRS_RELEASE )
set(catch2_FRAMEWORKS_RELEASE )
set(catch2_BUILD_DIRS_RELEASE "${catch2_PACKAGE_FOLDER_RELEASE}/lib/cmake/Catch2")
set(catch2_NO_SONAME_MODE_RELEASE FALSE)


# COMPOUND VARIABLES
set(catch2_COMPILE_OPTIONS_RELEASE
    "$<$<COMPILE_LANGUAGE:CXX>:${catch2_COMPILE_OPTIONS_CXX_RELEASE}>"
    "$<$<COMPILE_LANGUAGE:C>:${catch2_COMPILE_OPTIONS_C_RELEASE}>")
set(catch2_LINKER_FLAGS_RELEASE
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${catch2_SHARED_LINK_FLAGS_RELEASE}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${catch2_SHARED_LINK_FLAGS_RELEASE}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${catch2_EXE_LINK_FLAGS_RELEASE}>")


set(catch2_COMPONENTS_RELEASE Catch2::Catch2 Catch2::Catch2WithMain)
########### COMPONENT Catch2::Catch2WithMain VARIABLES ############################################

set(catch2_Catch2_Catch2WithMain_INCLUDE_DIRS_RELEASE "${catch2_PACKAGE_FOLDER_RELEASE}/include")
set(catch2_Catch2_Catch2WithMain_LIB_DIRS_RELEASE "${catch2_PACKAGE_FOLDER_RELEASE}/lib")
set(catch2_Catch2_Catch2WithMain_BIN_DIRS_RELEASE )
set(catch2_Catch2_Catch2WithMain_LIBRARY_TYPE_RELEASE STATIC)
set(catch2_Catch2_Catch2WithMain_IS_HOST_WINDOWS_RELEASE 0)
set(catch2_Catch2_Catch2WithMain_RES_DIRS_RELEASE )
set(catch2_Catch2_Catch2WithMain_DEFINITIONS_RELEASE )
set(catch2_Catch2_Catch2WithMain_OBJECTS_RELEASE )
set(catch2_Catch2_Catch2WithMain_COMPILE_DEFINITIONS_RELEASE )
set(catch2_Catch2_Catch2WithMain_COMPILE_OPTIONS_C_RELEASE "")
set(catch2_Catch2_Catch2WithMain_COMPILE_OPTIONS_CXX_RELEASE "")
set(catch2_Catch2_Catch2WithMain_LIBS_RELEASE Catch2Main)
set(catch2_Catch2_Catch2WithMain_SYSTEM_LIBS_RELEASE m)
set(catch2_Catch2_Catch2WithMain_FRAMEWORK_DIRS_RELEASE )
set(catch2_Catch2_Catch2WithMain_FRAMEWORKS_RELEASE )
set(catch2_Catch2_Catch2WithMain_DEPENDENCIES_RELEASE Catch2::Catch2)
set(catch2_Catch2_Catch2WithMain_SHARED_LINK_FLAGS_RELEASE )
set(catch2_Catch2_Catch2WithMain_EXE_LINK_FLAGS_RELEASE )
set(catch2_Catch2_Catch2WithMain_NO_SONAME_MODE_RELEASE FALSE)

# COMPOUND VARIABLES
set(catch2_Catch2_Catch2WithMain_LINKER_FLAGS_RELEASE
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${catch2_Catch2_Catch2WithMain_SHARED_LINK_FLAGS_RELEASE}>
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${catch2_Catch2_Catch2WithMain_SHARED_LINK_FLAGS_RELEASE}>
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${catch2_Catch2_Catch2WithMain_EXE_LINK_FLAGS_RELEASE}>
)
set(catch2_Catch2_Catch2WithMain_COMPILE_OPTIONS_RELEASE
    "$<$<COMPILE_LANGUAGE:CXX>:${catch2_Catch2_Catch2WithMain_COMPILE_OPTIONS_CXX_RELEASE}>"
    "$<$<COMPILE_LANGUAGE:C>:${catch2_Catch2_Catch2WithMain_COMPILE_OPTIONS_C_RELEASE}>")
########### COMPONENT Catch2::Catch2 VARIABLES ############################################

set(catch2_Catch2_Catch2_INCLUDE_DIRS_RELEASE "${catch2_PACKAGE_FOLDER_RELEASE}/include")
set(catch2_Catch2_Catch2_LIB_DIRS_RELEASE "${catch2_PACKAGE_FOLDER_RELEASE}/lib")
set(catch2_Catch2_Catch2_BIN_DIRS_RELEASE )
set(catch2_Catch2_Catch2_LIBRARY_TYPE_RELEASE STATIC)
set(catch2_Catch2_Catch2_IS_HOST_WINDOWS_RELEASE 0)
set(catch2_Catch2_Catch2_RES_DIRS_RELEASE )
set(catch2_Catch2_Catch2_DEFINITIONS_RELEASE )
set(catch2_Catch2_Catch2_OBJECTS_RELEASE )
set(catch2_Catch2_Catch2_COMPILE_DEFINITIONS_RELEASE )
set(catch2_Catch2_Catch2_COMPILE_OPTIONS_C_RELEASE "")
set(catch2_Catch2_Catch2_COMPILE_OPTIONS_CXX_RELEASE "")
set(catch2_Catch2_Catch2_LIBS_RELEASE Catch2)
set(catch2_Catch2_Catch2_SYSTEM_LIBS_RELEASE )
set(catch2_Catch2_Catch2_FRAMEWORK_DIRS_RELEASE )
set(catch2_Catch2_Catch2_FRAMEWORKS_RELEASE )
set(catch2_Catch2_Catch2_DEPENDENCIES_RELEASE )
set(catch2_Catch2_Catch2_SHARED_LINK_FLAGS_RELEASE )
set(catch2_Catch2_Catch2_EXE_LINK_FLAGS_RELEASE )
set(catch2_Catch2_Catch2_NO_SONAME_MODE_RELEASE FALSE)

# COMPOUND VARIABLES
set(catch2_Catch2_Catch2_LINKER_FLAGS_RELEASE
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${catch2_Catch2_Catch2_SHARED_LINK_FLAGS_RELEASE}>
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${catch2_Catch2_Catch2_SHARED_LINK_FLAGS_RELEASE}>
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${catch2_Catch2_Catch2_EXE_LINK_FLAGS_RELEASE}>
)
set(catch2_Catch2_Catch2_COMPILE_OPTIONS_RELEASE
    "$<$<COMPILE_LANGUAGE:CXX>:${catch2_Catch2_Catch2_COMPILE_OPTIONS_CXX_RELEASE}>"
    "$<$<COMPILE_LANGUAGE:C>:${catch2_Catch2_Catch2_COMPILE_OPTIONS_C_RELEASE}>")