# Load the debug and release variables
file(GLOB DATA_FILES "${CMAKE_CURRENT_LIST_DIR}/etl-*-data.cmake")

foreach(f ${DATA_FILES})
    include(${f})
endforeach()

# Create the targets for all the components
foreach(_COMPONENT ${etl_COMPONENT_NAMES} )
    if(NOT TARGET ${_COMPONENT})
        add_library(${_COMPONENT} INTERFACE IMPORTED)
        message(${etl_MESSAGE_MODE} "Conan: Component target declared '${_COMPONENT}'")
    endif()
endforeach()

if(NOT TARGET etl::etl)
    add_library(etl::etl INTERFACE IMPORTED)
    message(${etl_MESSAGE_MODE} "Conan: Target declared 'etl::etl'")
endif()
if(NOT TARGET etl)
    add_library(etl INTERFACE IMPORTED)
    set_property(TARGET etl PROPERTY INTERFACE_LINK_LIBRARIES etl::etl)
endif()
# Load the debug and release library finders
file(GLOB CONFIG_FILES "${CMAKE_CURRENT_LIST_DIR}/etl-Target-*.cmake")

foreach(f ${CONFIG_FILES})
    include(${f})
endforeach()