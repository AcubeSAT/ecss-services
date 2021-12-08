cmake_minimum_required(VERSION 3.14)
project(Logger)

set(CMAKE_CXX_STANDARD 17)

include_directories("${PROJECT_SOURCE_DIR}/inc" "${PROJECT_SOURCE_DIR}/lib/etl/include")
# Change the executable's name. It should match the project's name.
add_executable(Logger
        src/main.cpp
        src/Logger.cpp
        src/Platform/x86/Logger.cpp
        )