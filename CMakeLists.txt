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
add_executable(ecss_services src/main.cpp src/Message.cpp src/Service.cpp
        src/Services/TestService.cpp src/Services/RequestVerificationService.cpp
        src/Services/MemoryManagementService.cpp src/Services/ParameterService.cpp
        src/Services/TestService.cpp
        src/Services/RequestVerificationService.cpp)

IF (EXISTS "${PROJECT_SOURCE_DIR}/lib/Catch2/CMakeLists.txt")
    add_subdirectory(lib/Catch2)
    add_executable(tests src/Message.cpp src/Services/TestService.cpp
            src/Services/RequestVerificationService.cpp src/Services/ParameterService.cpp
            src/Services/MemoryManagementService.cpp test/tests.cpp test/Message.cpp
            test/TestPlatform.cpp test/Services/TestService.cpp
            test/Services/RequestVerificationService.cpp test/Services/ParameterService.cpp
            test/Services/MemoryManagementService.cpp)
    target_link_libraries(tests Catch2::Catch2)
ENDIF ()
