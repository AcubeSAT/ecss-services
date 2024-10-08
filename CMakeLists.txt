cmake_minimum_required(VERSION 3.18)
project(ecss_services)
set(CMAKE_CXX_STANDARD 17)

find_package(etl CONFIG REQUIRED)
find_package(logger CONFIG REQUIRED COMPONENTS log_common log_x86)

add_custom_target(check
        COMMAND ./cppcheck.sh
        COMMAND ./clang-tidy.sh
        COMMAND ./cppcheck-misra.sh
        WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/ci")

add_compile_options(-Wvla)
include_directories(${PLATFORM_DEFINITIONS_PATH})
add_library(common STATIC)
target_include_directories(common PUBLIC "${PROJECT_SOURCE_DIR}/inc/")
target_sources(common PRIVATE
        src/Service.cpp
        src/ErrorHandler.cpp
        src/Message.cpp
        src/MessageParser.cpp
        src/ServicePool.cpp
        src/Helpers/CRCHelper.cpp
        src/Helpers/PacketStore.cpp
        src/Time/UTCTimestamp.cpp
        src/Services/EventReportService.cpp
        src/Services/MemoryManagementService.cpp
        src/Services/ParameterService.cpp
        src/Services/RequestVerificationService.cpp
        src/Services/TestService.cpp
        src/Services/LargePacketTransferService.cpp
        src/Services/EventActionService.cpp
        src/Services/TimeBasedSchedulingService.cpp
        src/Services/FunctionManagementService.cpp
        src/Services/StorageAndRetrievalService.cpp
        src/Services/HousekeepingService.cpp
        src/Services/ParameterStatisticsService.cpp
        src/Services/OnBoardMonitoringService.cpp
        src/Helpers/Statistic.cpp
        src/Services/RealTimeForwardingControlService.cpp
        src/Helpers/PMON.cpp
        src/Helpers/AllReportTypes.cpp
        src/Helpers/FilepathValidators.cpp
        src/Services/FileManagementService.cpp
)

target_link_libraries(common
        PUBLIC etl log_common
        PRIVATE log_x86
)

# Specify the .cpp files for the executables
IF (X86_BUILD)
    file(GLOB_RECURSE x86_main_SRC "src/Platform/x86/*.cpp")

    add_executable(x86_services
            ${x86_main_SRC}
    )
    target_link_libraries(x86_services PRIVATE etl log_common log_x86 common)

    # Logs all levels of messages. This command can be added by other users of this
    # library to override the respective log level.
    target_compile_definitions(x86_services PRIVATE LOGLEVEL_TRACE)

    find_package(Catch2 CONFIG)
    IF(Catch2_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/test")
        file(GLOB test_main_SRC "test/*.cpp")
        file(GLOB test_SRC "test/**/*.cpp")
        add_executable(tests
                ${test_main_SRC}
                ${test_SRC})
        target_link_libraries(tests PRIVATE etl log_common log_x86 common Catch2::Catch2WithMain)
    ENDIF()
ENDIF()

install(TARGETS common)
