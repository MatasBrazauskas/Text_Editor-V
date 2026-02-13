#include "buffer/FilesAndCursor.hpp"
#include "buffer/Matrix.hpp"
#include "core/Editor.hpp"
#include "core/Renderer.hpp"
#include "utils/Config.hpp"
#include "utils/FileHandler.hpp"

using namespace std::string_literals;

const std::filesystem::path configPath = "config.json";

int main(const int argc, char** argv) {

	Config config(configPath);
	const auto ticksPerFrame = 1.0 / static_cast<double>(config.editor_.fps);

	/*FileHandler fileHandler;
	FilesAndCursor files{fileHandler, argc, argv};*/

	Editor editor{argc, argv};
	RenderScreen rendererScreen{editorState, files, config};

	for (auto i{0zu}; i < files.files_.size(); i++) {
		rendererScreen.addWindow(RenderWindow(0, 0, 0, 800, 600));
	}

	const Uint64 freq = SDL_GetPerformanceFrequency();
	Uint64 renderStart = SDL_GetPerformanceCounter();

	while (editorState.running_) {

		const Uint64 end = SDL_GetPerformanceCounter();
		const double renderTime = static_cast<double>(end - renderStart) / static_cast<double>(freq);

		editor.HandleKeyboardInput();

		if (renderTime >= ticksPerFrame) {
			renderStart = end;
			rendererScreen.Render();
		}
	}
}
