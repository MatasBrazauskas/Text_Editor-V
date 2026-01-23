cmake_minimum_required(VERSION 4.0)
project(Text_Editor)

set(CMAKE_CXX_STANDARD 23)

find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)

add_executable(Text_Editor main.cpp)

add_subdirectory(src/buffer)
add_subdirectory(src/commands)
add_subdirectory(src/core)
add_subdirectory(src/utils)

target_link_libraries(Text_Editor PRIVATE SDL2::SDL2 SDL2_ttf::SDL2_ttf)

target_include_directories(Text_Editor PRIVATE src src/graphics scr/includes)

set_target_properties(Text_Editor PROPERTIES CXX_STANDARD 23 CXX_STANDARD_REQUIRE ON CXX_EXTENSIONS OFF)
