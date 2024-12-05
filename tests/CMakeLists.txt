enable_testing()

# Find all test files
file(GLOB_RECURSE TEST_SOURCES "*.cpp")

# Create test executable
add_executable(unit_tests ${TEST_SOURCES})

# Link with Google Test
find_package(GTest REQUIRED)
target_link_libraries(unit_tests PRIVATE GTest::GTest GTest::Main)

# Add test to CTest
add_test(NAME unit_tests COMMAND unit_tests)

# Set test properties
set_tests_properties(unit_tests PROPERTIES
    ENVIRONMENT "PATH=${CMAKE_BINARY_DIR}/bin:$ENV{PATH}"
)
