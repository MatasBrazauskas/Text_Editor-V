#include "core/EditorTextArea.hpp"
#include <iostream>

#include "src/graphics/Sdl.hpp"
#include "src/graphics/SdlInput.hpp"

#include "src/utils/Config.hpp"
#include "src/utils/FileHandler.hpp"

#include "buffer/Matrix.hpp"

constexpr double Fps = 30;
constexpr double ticksPerFrame = 1.0 / Fps;


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
    //Graphics::SdlWindow window{title};

    //Graphics::SdlTtf font{config.font_};

    Sdl sdl{title, (config.font_.font_path.c_str()), config.font_.font_size};
    SdlInput input{};

    EditorTextArea editorTextArea {config, matrix, sdl};

    const Uint64 freq = SDL_GetPerformanceFrequency();
    Uint64 frameStart = SDL_GetPerformanceCounter();

    while (true) {

        Uint64 frameEnd = SDL_GetPerformanceCounter();
        double frameTime = static_cast<double>(frameEnd - frameStart) / freq;

        const auto captured = input.getInput();

        if (!captured.empty()) {
            SDL_Log("Input captured: %s", captured.c_str());
        }
        if (frameTime >= ticksPerFrame) {
            frameStart = frameEnd;
            editorTextArea.RenderText();
        }
    }
}