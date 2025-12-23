#include "Renderer.hpp"
#include "src/graphics/Sdl.hpp"

#include "src/utils/Config.hpp"
#include "src/utils/FileHandler.hpp"

#include "buffer/Matrix.hpp"

#include "buffer/Cursor.hpp"

#include "core/Editor.hpp"

constexpr auto Fps = 60.0;
constexpr auto ticksPerFrame = 1.0 / Fps;

constexpr double InputReset = 4.0;

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
    cursor.visible_ = true;

    Editor editor {matrix, cursor};

    //Graphics
    Sdl sdl{config};
    Renderer renderer{matrix, cursor, config, sdl};

    const Uint64 freq = SDL_GetPerformanceFrequency();
    Uint64 renderStart = SDL_GetPerformanceCounter();

    while (editor.running_) {

        Uint64 end = SDL_GetPerformanceCounter();
        double renderTime = static_cast<double>(end - renderStart) / freq;

        editor.HandleKeyboardInput();

        if (renderTime >= ticksPerFrame) {
            renderStart = end;
            renderer.Render();
        }
    }
}