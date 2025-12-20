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
        src/commands/Insert.hpp
        src/commands/Normal.cpp
        src/commands/Normal.hpp
        src/commands/ICommand.hpp
        src/core/EditorTextArea.cpp
        src/core/EditorTextArea.hpp
        src/graphics/Sdl.cpp
        src/graphics/Sdl.hpp
        src/utils/FileHandler.hpp
        src/buffer/Matrix.hpp
        src/buffer/Cursor.cpp
        src/buffer/Cursor.h
        src/graphics/SdlInput.cpp
        src/graphics/SdlInput.hpp
)

target_link_libraries(Text_Editor PRIVATE
        SDL2::SDL2
        SDL2_ttf::SDL2_ttf
)

target_include_directories(Text_Editor PRIVATE src src/graphics)
