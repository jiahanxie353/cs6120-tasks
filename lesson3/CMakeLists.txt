cmake_minimum_required(VERSION 3.10)

# Set the project name
project(LocalOpt)

# Add the include directories
include_directories(${CMAKE_SOURCE_DIR})

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add source files
set(SOURCE_FILES
    form_blocks.cpp
    global_unused_instrs.cpp
    local_kill_instrs.cpp
    local_value_number.cpp
    local_analysis_opt.cpp
    ../utils.cpp
)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../)

# Add executable
add_executable(local_opt ${SOURCE_FILES})
