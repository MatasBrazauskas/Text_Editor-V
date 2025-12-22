#include <iostream>

#include "Renderer.hpp"
#include "src/graphics/Sdl.hpp"

#include "src/utils/Config.hpp"
#include "src/utils/FileHandler.hpp"

#include "buffer/Matrix.hpp"

#include "buffer/Cursor.hpp"

#include "commands/Commands.hpp"
#include "core/Editor.hpp"

constexpr auto Fps = 60.0;
constexpr auto ticksPerFrame = 1.0 / Fps;


int main() {
    //Config
    const std::filesystem::path configPath = "config.json";
    Config config (configPath);

    //Content
    const auto filesPath = "text.txt";
    FileHandler fileHandler {filesPath};
    const auto lines = fileHandler.getContent();

    Matrix matrix;
    matrix.init(lines);

    Cursor cursor{matrix};
    Commands commands{};

    Editor editor {matrix, cursor, config, commands};

    //Graphics
    Sdl sdl{config};
    Renderer renderer{matrix, cursor, config, sdl};

    const Uint64 freq = SDL_GetPerformanceFrequency();
    Uint64 renderStart = SDL_GetPerformanceCounter();

    while (true) {

        Uint64 end = SDL_GetPerformanceCounter();
        double frameTime = static_cast<double>(end - renderStart) / freq;

        editor.HandleKeyboardInput();

        if (frameTime >= ticksPerFrame) {
            renderStart = end;
            renderer.RenderText();
        }
    }
}