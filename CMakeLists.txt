cmake_minimum_required(VERSION 3.7)
project(ecss_services)

# Set C++ version to c++17
set(CMAKE_CXX_STANDARD 17)

# Specify the directories for #includes
include_directories("${PROJECT_SOURCE_DIR}/inc")

add_custom_target(check
        COMMAND ./cppcheck.sh
        COMMAND ./vera.sh
        COMMAND ./clang-tidy.sh
        WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/ci")

# Specify the .cpp files for the executables
add_executable(ecss_services src/main.cpp src/Message.cpp src/Service.cpp src/Services/TestService.cpp)

IF(EXISTS lib/Catch2)
add_subdirectory(lib/Catch2)
add_executable(tests src/Message.cpp src/Service.cpp src/Services/TestService.cpp tests/tests.cpp tests/Message.cpp)
target_link_libraries(tests Catch2::Catch2)
ENDIF()
