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

if(NOT MSVC)
    add_compile_options(-Wvla) 
endif()

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

# Logger::log comes from the platform (log_x86 or TestPlatform), not this library.
target_link_libraries(common
        PUBLIC etl log_common
)
target_compile_definitions(common PRIVATE LOGLEVEL_ERROR)

if(ECSS_REPORT_STATIC_SIZES AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    if(DEFINED ENV{ECSS_SIZE_REPORT_DIR})
        set(ECSS_SIZE_REPORT_DIR "$ENV{ECSS_SIZE_REPORT_DIR}")
    else()
        set(ECSS_SIZE_REPORT_DIR "${CMAKE_BINARY_DIR}/service-sizes")
    endif()
    file(MAKE_DIRECTORY "${ECSS_SIZE_REPORT_DIR}")
    file(WRITE "${CMAKE_BINARY_DIR}/service_size_dump.cpp" "#include \"ServicePool.hpp\"\n")

    add_library(service_size_dump OBJECT "${CMAKE_BINARY_DIR}/service_size_dump.cpp")
    target_link_libraries(service_size_dump PRIVATE common)
    target_compile_options(service_size_dump PRIVATE
            "-fdump-lang-class=${ECSS_SIZE_REPORT_DIR}/service_pool.class")

    add_custom_command(
            OUTPUT "${ECSS_SIZE_REPORT_DIR}/service_sizes.txt"
            DEPENDS service_size_dump
            COMMAND python3 "${PROJECT_SOURCE_DIR}/ci/report_service_sizes.py"
            --dump "${ECSS_SIZE_REPORT_DIR}/service_pool.class"
            --label "$ENV{ECSS_SIZE_REPORT_LABEL}"
            --output "${ECSS_SIZE_REPORT_DIR}/service_sizes.txt"
            COMMAND ${CMAKE_COMMAND} -E cat "${ECSS_SIZE_REPORT_DIR}/service_sizes.txt"
            COMMENT "Reporting static ServicePool and per-service object sizes"
            VERBATIM)
    add_custom_target(report_service_sizes ALL DEPENDS "${ECSS_SIZE_REPORT_DIR}/service_sizes.txt")
endif()

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
        file(GLOB test_x86_shared_SRC "src/Platform/x86/Filesystem.cpp" "src/Platform/x86/TestMemory.cpp" "src/Platform/x86/Services/MemoryManagementService.cpp")

        add_executable(tests
                ${test_x86_shared_SRC}
                ${test_main_SRC}
                ${test_SRC})
        target_link_libraries(tests PRIVATE etl log_common common Catch2::Catch2WithMain)
    ENDIF()
ENDIF()
if(MSVC)
    target_compile_options(common PUBLIC /permissive-)
endif()
install(TARGETS common)
