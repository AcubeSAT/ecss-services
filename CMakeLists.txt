cmake_minimum_required(VERSION 3.7)
project(ecss_services)

# Set C++ version to c++17
set(CMAKE_CXX_STANDARD 17)

# Specify the directories for #includes
include_directories("${PROJECT_SOURCE_DIR}/inc" "${PROJECT_SOURCE_DIR}/lib/etl/include")

add_custom_target(check
        COMMAND ./cppcheck.sh
        COMMAND ./clang-tidy.sh
        COMMAND ./cppcheck-misra.sh
        WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/ci")

# Allow the user to set the ECSS Configuration directory using the -DECSS_CONFIGURATION=... command line argument
set(ECSS_CONFIGURATION "${PROJECT_SOURCE_DIR}/inc/Platform/x86" CACHE PATH
        "The include directory for the platform-specific ECSS configuration headers"
        )
include_directories(${ECSS_CONFIGURATION})

# Specify the .cpp files common across all targets
add_library(common OBJECT
        src/Service.cpp
        src/ErrorHandler.cpp
        src/Logger.cpp
        src/Message.cpp
        src/MessageParser.cpp
        src/ServicePool.cpp
        src/Helpers/CRCHelper.cpp
        src/Helpers/TimeAndDate.cpp
        src/Helpers/TimeHelper.cpp
        src/Platform/x86/Parameters/SystemParameters.cpp
        src/Services/EventReportService.cpp
        src/Services/MemoryManagementService.cpp
        src/Services/ParameterService.cpp
        src/Services/RequestVerificationService.cpp
        src/Services/TestService.cpp
        src/Services/LargePacketTransferService.cpp
    	src/Services/EventActionService.cpp
        src/Services/TimeBasedSchedulingService.cpp
        src/Services/FunctionManagementService.cpp
        )

# Specify the .cpp files for the executables
file(GLOB x86_main_SRC "src/Platform/x86/*.cpp")
add_executable(ecss_services
        $<TARGET_OBJECTS:common>
        ${x86_main_SRC}
        )

IF (EXISTS "${PROJECT_SOURCE_DIR}/lib/Catch2/CMakeLists.txt")
    # Gather all the .cpp files corresponding to tests
    file(GLOB test_main_SRC "test/*.cpp")
    file(GLOB test_SRC "test/**/*.cpp")

    add_subdirectory(lib/Catch2)
    add_executable(tests
            $<TARGET_OBJECTS:common>
            ${test_main_SRC}
            ${test_SRC}
            )
    target_link_libraries(tests Catch2::Catch2)
ENDIF ()
