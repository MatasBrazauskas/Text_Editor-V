cmake_minimum_required(VERSION 3.28)
project(Text_Editor)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 1. Dependencies
find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)

# 2. GoogleTest Setup
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# 3. Source Collection
# We exclude main.cpp from the library so tests don't have multiple mains
file(GLOB_RECURSE LIB_SOURCES
        "src/buffer/*.cpp"
        "src/commands/*.cpp"
        "src/core/*.cpp"
        "src/utils/*.cpp"
)

# 4. Define Logic Library
add_library(EditorLogic STATIC ${LIB_SOURCES}
        src/buffer/Panes.cpp
        src/buffer/Panes.hpp)
# 5. Link Dependencies to the Library
# We use PUBLIC so that anyone linking to EditorLogic (like main or tests)
# automatically gets the SDL include paths and libraries.
target_link_libraries(EditorLogic PUBLIC
        SDL2::SDL2
        SDL2_ttf::SDL2_ttf
)

# 6. Include Directories
# Use PUBLIC so tests can see your headers
target_include_directories(EditorLogic PUBLIC
        src
        src/includes
)

# 7. Main Executable
add_executable(Text_Editor main.cpp)
target_link_libraries(Text_Editor PRIVATE EditorLogic)

# 8. Testing
enable_testing()
add_subdirectory(tests)
