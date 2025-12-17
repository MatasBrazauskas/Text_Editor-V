cmake_minimum_required(VERSION 4.1)
project(Text_Editor)

set(CMAKE_CXX_STANDARD 23)

find_package(SDL2 REQUIRED)

add_executable(Text_Editor
        main.cpp
        src/buffer/ITextBuffer.hpp
        src/buffer/Matrix.cpp
        src/buffer/Matrix.hpp
        src/wrappers/Config.hpp
        src/wrappers/SdlWrapper.hpp
        src/wrappers/SdlWrapper.cpp
        src/includes/Json.hpp
        src/wrappers/Config.cpp
)

target_link_libraries(Text_Editor PRIVATE SDL2::SDL2)

target_include_directories(Text_Editor PRIVATE src src/wrappers)
