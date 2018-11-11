cmake_minimum_required(VERSION 3.9)
project(ecss_services)

# Set C++ version to c++17
set(CMAKE_CXX_STANDARD 17)

# Specify the directories for #includes
include_directories ("${PROJECT_SOURCE_DIR}/inc")

# Specify the .cpp files for the executables
add_executable(ecss_services src/main.cpp src/Message.cpp)
