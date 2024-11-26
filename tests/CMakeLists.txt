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
