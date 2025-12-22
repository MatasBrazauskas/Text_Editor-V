#include "core/EditorTextArea.hpp"
#include <iostream>

#include "src/graphics/Sdl.hpp"

#include "src/utils/Config.hpp"
#include "src/utils/FileHandler.hpp"

#include "buffer/Matrix.hpp"

#include "commands/Commands.hpp"

constexpr auto Fps = 60.0;
constexpr auto ticksPerFrame = 1.0 / Fps;


int main() {
    //Config
    const std::filesystem::path configPath = "config.json";
    Config config (configPath);

    //Text
    const auto filesPath = "text.txt";
    FileHandler fileHandler {filesPath};
    const auto lines = fileHandler.getContent();

    Matrix matrix;
    matrix.init(lines);

    //Graphics
    const auto title = "Text Editor";
    Sdl sdl{title, (config.font_.font_path.c_str()), config.font_.font_size};

    EditorTextArea editorTextArea {config, matrix, sdl};

    //Commands
    Commands commands{};

    Cursor cursor{config};

    const Uint64 freq = SDL_GetPerformanceFrequency();
    Uint64 renderStart = SDL_GetPerformanceCounter();
    Uint64 cursorStart = SDL_GetPerformanceCounter();

    while (true) {

        Uint64 end = SDL_GetPerformanceCounter();
        double frameTime = static_cast<double>(end - renderStart) / freq;
        double cursorTime = static_cast<double>(end - cursorStart) / freq;

        commands.HandleInput();

        if (cursorTime >= 0.5) {
            std::cout << "Is it runnder?\n";
            cursorStart = end;
            cursor.state = !cursor.state;
            cursor.RenderCursor(sdl);
        }

        /*if (frameTime >= ticksPerFrame) {
            renderStart = end;
            editorTextArea.RenderText();
        }*/
    }
}