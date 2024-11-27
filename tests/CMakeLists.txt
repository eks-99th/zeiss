# Google Test setup
include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.tar.gz
)

# Download and add Google Test
FetchContent_MakeAvailable(googletest)

# Add the test executable
add_executable(UnitTests test_exs.cpp)

# Link with Google Test, Google Mock, and the library
target_link_libraries(UnitTests MyLibrary gtest gmock gtest_main gmock_main)

# Add test cases to CTest
include(GoogleTest)
gtest_discover_tests(UnitTests)

# Define the source and destination of the test  directories
set(TESTS_SOURCE_DIR "${CMAKE_SOURCE_DIR}/tests/test_data")
set(TESTS_BUILD_DIR "${CMAKE_BINARY_DIR}/tests")

# Copy test data files to the build directory
file(COPY ${TESTS_SOURCE_DIR}/medium.txt DESTINATION ${TESTS_BUILD_DIR})
file(COPY ${TESTS_SOURCE_DIR}/small.txt DESTINATION ${TESTS_BUILD_DIR})
file(COPY ${TESTS_SOURCE_DIR}/big.txt DESTINATION ${TESTS_BUILD_DIR})
