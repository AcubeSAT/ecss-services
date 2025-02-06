# Load the debug and release variables
file(GLOB DATA_FILES "${CMAKE_CURRENT_LIST_DIR}/Catch2-*-data.cmake")

foreach(f ${DATA_FILES})
    include(${f})
endforeach()

# Create the targets for all the components
foreach(_COMPONENT ${catch2_COMPONENT_NAMES} )
    if(NOT TARGET ${_COMPONENT})
        add_library(${_COMPONENT} INTERFACE IMPORTED)
        message(${Catch2_MESSAGE_MODE} "Conan: Component target declared '${_COMPONENT}'")
    endif()
endforeach()

if(NOT TARGET Catch2::Catch2WithMain)
    add_library(Catch2::Catch2WithMain INTERFACE IMPORTED)
    message(${Catch2_MESSAGE_MODE} "Conan: Target declared 'Catch2::Catch2WithMain'")
endif()
# Load the debug and release library finders
file(GLOB CONFIG_FILES "${CMAKE_CURRENT_LIST_DIR}/Catch2-Target-*.cmake")

foreach(f ${CONFIG_FILES})
    include(${f})
endforeach()