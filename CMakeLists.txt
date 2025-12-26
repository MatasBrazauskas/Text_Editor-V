cmake_minimum_required(VERSION 4.1)
project(Text_Editor)

set(CMAKE_CXX_STANDARD 23)

find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)

add_executable(Text_Editor
        main.cpp
        src/buffer/ITextBuffer.hpp
        src/buffer/Matrix.cpp
        src/includes/Json.hpp
        src/utils/Config.hpp
        src/utils/Config.cpp
        src/utils/FileHandler.cpp
        src/commands/Insert.cpp
        src/commands/Normal.cpp
        src/graphics/Sdl.cpp
        src/graphics/Sdl.hpp
        src/utils/FileHandler.hpp
        src/buffer/Matrix.hpp
        src/buffer/Cursor.cpp
        src/buffer/Cursor.hpp
        src/core/Editor.cpp
        src/core/Editor.hpp
        src/graphics/Renderer.cpp
        src/graphics/Renderer.hpp
        src/commands/IMode.hpp
        src/commands/Insert.hpp
        src/commands/Normal.hpp
        src/commands/CommandMode.cpp
        src/commands/CommandMode.hpp
)

target_link_libraries(Text_Editor PRIVATE
        SDL2::SDL2
        SDL2_ttf::SDL2_ttf
)

target_include_directories(Text_Editor PRIVATE src src/graphics)
