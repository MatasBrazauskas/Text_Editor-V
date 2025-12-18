cmake_minimum_required(VERSION 4.1)
project(Text_Editor)

set(CMAKE_CXX_STANDARD 23)

find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)

add_executable(Text_Editor
        main.cpp
        src/buffer/ITextBuffer.hpp
        src/buffer/Matrix.cpp
        src/wrappers/SdlWindow.hpp
        src/wrappers/SdlWindow.cpp
        src/includes/Json.hpp
        src/utils/Config.hpp
        src/utils/Config.cpp
        src/utils/FileHandler.cpp
        src/commands/Insert.cpp
        src/commands/Insert.hpp
        src/commands/Visual.cpp
        src/commands/Visual.hpp
        src/commands/Normal.cpp
        src/commands/Normal.hpp
        src/commands/ICommand.hpp
        src/wrappers/SdlTtf.cpp
        src/wrappers/SdlTtf.h
)

target_link_libraries(Text_Editor PRIVATE
        SDL2::SDL2
        SDL2_ttf::SDL2_ttf
)

target_include_directories(Text_Editor PRIVATE src src/wrappers)
