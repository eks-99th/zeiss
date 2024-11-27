cmake_minimum_required(VERSION 3.10)
project(MyProject)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# FetchContent for argparse
include(FetchContent)

FetchContent_Declare(
    argparse
    GIT_REPOSITORY https://github.com/p-ranav/argparse.git
    GIT_TAG v2.9  # Replace with the desired version or branch
)

FetchContent_MakeAvailable(argparse)

# Include directories
include_directories(include)
include_directories(interfaces)

# Create a library from the source files
add_library(MyLibrary src/exs.cpp)

# Add executable
add_executable(esx src/main.cpp)
message(STATUS "argparse_SOURCE_DIR: ${argparse_SOURCE_DIR}")
target_include_directories(esx PRIVATE ${argparse_SOURCE_DIR}/include/argparse)

# Find and link LibSerial
find_library(LibSerial_LIB serial REQUIRED)
target_include_directories(esx PRIVATE /usr/include/libserial)
target_link_libraries(esx PRIVATE ${LibSerial_LIB} MyLibrary)

# Add Google Test
enable_testing()
add_subdirectory(tests)
