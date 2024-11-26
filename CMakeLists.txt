cmake_minimum_required(VERSION 3.10)
project(MyProject)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include directories
include_directories(include)
include_directories(interfaces)

# Create a library from the source files
add_library(MyLibrary src/exs.cpp)

# Add executable
add_executable(serial_example src/main.cpp)

# Find and link LibSerial
find_library(LibSerial_LIB serial REQUIRED)
target_include_directories(serial_example PRIVATE /usr/include/libserial)
target_link_libraries(serial_example PRIVATE ${LibSerial_LIB} MyLibrary)

# Add Google Test
# enable_testing()
# add_subdirectory(tests)
