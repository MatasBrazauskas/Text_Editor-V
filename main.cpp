#include "src/core/Renderer.hpp"

#include "src/utils/Config.hpp"
#include "src/utils/FileHandler.hpp"
#include "buffer/Matrix.hpp"
#include "buffer/Files.hpp"
#include "core/Editor.hpp"

#include <iostream>

using namespace std::string_literals;

constexpr auto Fps           = 60.0;
constexpr auto ticksPerFrame = 1.0 / Fps;

const auto separators = " ,./?<>!@#$%^&*()_-+=|[]{}:'"s;

const std::filesystem::path configPath = "config.json";

int main(int argc, char** argv) {
	Config config(configPath);

	FileHandler fileHandler;
	Files files{fileHandler, argc, argv};

	EditorState editorState;
	Editor editor{files, editorState};
	Renderer renderer{editorState, files, config};

	const Uint64 freq = SDL_GetPerformanceFrequency();
	Uint64 renderStart = SDL_GetPerformanceCounter();

	while (editorState.running_) {

		Uint64 end        = SDL_GetPerformanceCounter();
		double renderTime = static_cast<double>(end - renderStart) / static_cast<double>(freq);

		editor.HandleKeyboardInput();

		if (renderTime >= ticksPerFrame) {
			renderStart = end;
			renderer.Render();
		}
	}
}