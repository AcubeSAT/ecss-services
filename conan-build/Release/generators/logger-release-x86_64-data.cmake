########### AGGREGATED COMPONENTS AND DEPENDENCIES FOR THE MULTI CONFIG #####################
#############################################################################################

list(APPEND logger_COMPONENT_NAMES log_common log_x86)
list(REMOVE_DUPLICATES logger_COMPONENT_NAMES)
if(DEFINED logger_FIND_DEPENDENCY_NAMES)
  list(APPEND logger_FIND_DEPENDENCY_NAMES etl)
  list(REMOVE_DUPLICATES logger_FIND_DEPENDENCY_NAMES)
else()
  set(logger_FIND_DEPENDENCY_NAMES etl)
endif()
set(etl_FIND_MODE "NO_MODULE")

########### VARIABLES #######################################################################
#############################################################################################
set(logger_PACKAGE_FOLDER_RELEASE "/root/.conan2/p/loggee0588c687a22f/p")
set(logger_BUILD_MODULES_PATHS_RELEASE )


set(logger_INCLUDE_DIRS_RELEASE "${logger_PACKAGE_FOLDER_RELEASE}/inc")
set(logger_RES_DIRS_RELEASE )
set(logger_DEFINITIONS_RELEASE )
set(logger_SHARED_LINK_FLAGS_RELEASE )
set(logger_EXE_LINK_FLAGS_RELEASE )
set(logger_OBJECTS_RELEASE )
set(logger_COMPILE_DEFINITIONS_RELEASE )
set(logger_COMPILE_OPTIONS_C_RELEASE )
set(logger_COMPILE_OPTIONS_CXX_RELEASE )
set(logger_LIB_DIRS_RELEASE "${logger_PACKAGE_FOLDER_RELEASE}/lib")
set(logger_BIN_DIRS_RELEASE )
set(logger_LIBRARY_TYPE_RELEASE STATIC)
set(logger_IS_HOST_WINDOWS_RELEASE 0)
set(logger_LIBS_RELEASE log_x86 log_common)
set(logger_SYSTEM_LIBS_RELEASE )
set(logger_FRAMEWORK_DIRS_RELEASE )
set(logger_FRAMEWORKS_RELEASE )
set(logger_BUILD_DIRS_RELEASE )
set(logger_NO_SONAME_MODE_RELEASE FALSE)


# COMPOUND VARIABLES
set(logger_COMPILE_OPTIONS_RELEASE
    "$<$<COMPILE_LANGUAGE:CXX>:${logger_COMPILE_OPTIONS_CXX_RELEASE}>"
    "$<$<COMPILE_LANGUAGE:C>:${logger_COMPILE_OPTIONS_C_RELEASE}>")
set(logger_LINKER_FLAGS_RELEASE
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${logger_SHARED_LINK_FLAGS_RELEASE}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${logger_SHARED_LINK_FLAGS_RELEASE}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${logger_EXE_LINK_FLAGS_RELEASE}>")


set(logger_COMPONENTS_RELEASE log_common log_x86)
########### COMPONENT log_x86 VARIABLES ############################################

set(logger_log_x86_INCLUDE_DIRS_RELEASE "${logger_PACKAGE_FOLDER_RELEASE}/inc")
set(logger_log_x86_LIB_DIRS_RELEASE "${logger_PACKAGE_FOLDER_RELEASE}/lib")
set(logger_log_x86_BIN_DIRS_RELEASE )
set(logger_log_x86_LIBRARY_TYPE_RELEASE STATIC)
set(logger_log_x86_IS_HOST_WINDOWS_RELEASE 0)
set(logger_log_x86_RES_DIRS_RELEASE )
set(logger_log_x86_DEFINITIONS_RELEASE )
set(logger_log_x86_OBJECTS_RELEASE )
set(logger_log_x86_COMPILE_DEFINITIONS_RELEASE )
set(logger_log_x86_COMPILE_OPTIONS_C_RELEASE "")
set(logger_log_x86_COMPILE_OPTIONS_CXX_RELEASE "")
set(logger_log_x86_LIBS_RELEASE log_x86)
set(logger_log_x86_SYSTEM_LIBS_RELEASE )
set(logger_log_x86_FRAMEWORK_DIRS_RELEASE )
set(logger_log_x86_FRAMEWORKS_RELEASE )
set(logger_log_x86_DEPENDENCIES_RELEASE )
set(logger_log_x86_SHARED_LINK_FLAGS_RELEASE )
set(logger_log_x86_EXE_LINK_FLAGS_RELEASE )
set(logger_log_x86_NO_SONAME_MODE_RELEASE FALSE)

# COMPOUND VARIABLES
set(logger_log_x86_LINKER_FLAGS_RELEASE
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${logger_log_x86_SHARED_LINK_FLAGS_RELEASE}>
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${logger_log_x86_SHARED_LINK_FLAGS_RELEASE}>
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${logger_log_x86_EXE_LINK_FLAGS_RELEASE}>
)
set(logger_log_x86_COMPILE_OPTIONS_RELEASE
    "$<$<COMPILE_LANGUAGE:CXX>:${logger_log_x86_COMPILE_OPTIONS_CXX_RELEASE}>"
    "$<$<COMPILE_LANGUAGE:C>:${logger_log_x86_COMPILE_OPTIONS_C_RELEASE}>")
########### COMPONENT log_common VARIABLES ############################################

set(logger_log_common_INCLUDE_DIRS_RELEASE "${logger_PACKAGE_FOLDER_RELEASE}/inc")
set(logger_log_common_LIB_DIRS_RELEASE "${logger_PACKAGE_FOLDER_RELEASE}/lib")
set(logger_log_common_BIN_DIRS_RELEASE )
set(logger_log_common_LIBRARY_TYPE_RELEASE STATIC)
set(logger_log_common_IS_HOST_WINDOWS_RELEASE 0)
set(logger_log_common_RES_DIRS_RELEASE )
set(logger_log_common_DEFINITIONS_RELEASE )
set(logger_log_common_OBJECTS_RELEASE )
set(logger_log_common_COMPILE_DEFINITIONS_RELEASE )
set(logger_log_common_COMPILE_OPTIONS_C_RELEASE "")
set(logger_log_common_COMPILE_OPTIONS_CXX_RELEASE "")
set(logger_log_common_LIBS_RELEASE log_common)
set(logger_log_common_SYSTEM_LIBS_RELEASE )
set(logger_log_common_FRAMEWORK_DIRS_RELEASE )
set(logger_log_common_FRAMEWORKS_RELEASE )
set(logger_log_common_DEPENDENCIES_RELEASE )
set(logger_log_common_SHARED_LINK_FLAGS_RELEASE )
set(logger_log_common_EXE_LINK_FLAGS_RELEASE )
set(logger_log_common_NO_SONAME_MODE_RELEASE FALSE)

# COMPOUND VARIABLES
set(logger_log_common_LINKER_FLAGS_RELEASE
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${logger_log_common_SHARED_LINK_FLAGS_RELEASE}>
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${logger_log_common_SHARED_LINK_FLAGS_RELEASE}>
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${logger_log_common_EXE_LINK_FLAGS_RELEASE}>
)
set(logger_log_common_COMPILE_OPTIONS_RELEASE
    "$<$<COMPILE_LANGUAGE:CXX>:${logger_log_common_COMPILE_OPTIONS_CXX_RELEASE}>"
    "$<$<COMPILE_LANGUAGE:C>:${logger_log_common_COMPILE_OPTIONS_C_RELEASE}>")