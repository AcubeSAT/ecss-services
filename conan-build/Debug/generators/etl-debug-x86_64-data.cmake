########### AGGREGATED COMPONENTS AND DEPENDENCIES FOR THE MULTI CONFIG #####################
#############################################################################################

set(etl_COMPONENT_NAMES "")
if(DEFINED etl_FIND_DEPENDENCY_NAMES)
  list(APPEND etl_FIND_DEPENDENCY_NAMES )
  list(REMOVE_DUPLICATES etl_FIND_DEPENDENCY_NAMES)
else()
  set(etl_FIND_DEPENDENCY_NAMES )
endif()

########### VARIABLES #######################################################################
#############################################################################################
set(etl_PACKAGE_FOLDER_DEBUG "/Users/claudemacmini/.conan2/p/b/etl850e3b9e24b4f/p")
set(etl_BUILD_MODULES_PATHS_DEBUG )


set(etl_INCLUDE_DIRS_DEBUG "${etl_PACKAGE_FOLDER_DEBUG}/include")
set(etl_RES_DIRS_DEBUG )
set(etl_DEFINITIONS_DEBUG )
set(etl_SHARED_LINK_FLAGS_DEBUG )
set(etl_EXE_LINK_FLAGS_DEBUG )
set(etl_OBJECTS_DEBUG )
set(etl_COMPILE_DEFINITIONS_DEBUG )
set(etl_COMPILE_OPTIONS_C_DEBUG )
set(etl_COMPILE_OPTIONS_CXX_DEBUG )
set(etl_LIB_DIRS_DEBUG )
set(etl_BIN_DIRS_DEBUG )
set(etl_LIBRARY_TYPE_DEBUG UNKNOWN)
set(etl_IS_HOST_WINDOWS_DEBUG 0)
set(etl_LIBS_DEBUG )
set(etl_SYSTEM_LIBS_DEBUG )
set(etl_FRAMEWORK_DIRS_DEBUG )
set(etl_FRAMEWORKS_DEBUG )
set(etl_BUILD_DIRS_DEBUG "${etl_PACKAGE_FOLDER_DEBUG}/lib/cmake")
set(etl_NO_SONAME_MODE_DEBUG FALSE)


# COMPOUND VARIABLES
set(etl_COMPILE_OPTIONS_DEBUG
    "$<$<COMPILE_LANGUAGE:CXX>:${etl_COMPILE_OPTIONS_CXX_DEBUG}>"
    "$<$<COMPILE_LANGUAGE:C>:${etl_COMPILE_OPTIONS_C_DEBUG}>")
set(etl_LINKER_FLAGS_DEBUG
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${etl_SHARED_LINK_FLAGS_DEBUG}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${etl_SHARED_LINK_FLAGS_DEBUG}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${etl_EXE_LINK_FLAGS_DEBUG}>")


set(etl_COMPONENTS_DEBUG )