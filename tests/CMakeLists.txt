# Create the test executable
add_executable(editor_tests normalModeCommandParsingTest.cpp)

# This command is what fixes "Cannot find directory 'gtest'"
target_link_libraries(editor_tests
        PRIVATE
        GTest::gtest_main   # This provides the include paths and the main() function
        EditorLogic         # This provides your Vim editor code
)

# Enable discovery so ctest can see the tests
include(GoogleTest)
gtest_discover_tests(editor_tests)