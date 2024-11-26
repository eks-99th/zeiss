cmake_minimum_required(VERSION 3.10)
project(MyProject)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include directories
include_directories(include)

# Create a library from the source files
add_library(MyLibrary src/example.cpp)

# Add the main executable
# add_executable(MyProject src/main.cpp)

# Add Google Test
enable_testing()
add_subdirectory(tests)
