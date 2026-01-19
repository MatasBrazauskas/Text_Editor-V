#include "buffer/Files.hpp"
#include "buffer/Matrix.hpp"
#include "core/Editor.hpp"
#include "core/Renderer.hpp"
#include "utils/Config.hpp"
#include "utils/FileHandler.hpp"

using namespace std::string_literals;

const std::filesystem::path configPath = "config.json";

int main(const int argc, char** argv) {
	Config	   config(configPath);
	const auto ticksPerFrame = 1.0 / static_cast<double>(config.editor_.fps);

	FileHandler fileHandler;
	Files	    files{fileHandler, argc, argv};

	EditorState editorState;
	Editor	    editor{files, fileHandler, editorState};
	Renderer    renderer{editorState, files, config};

	const Uint64 freq	 = SDL_GetPerformanceFrequency();
	Uint64	     renderStart = SDL_GetPerformanceCounter();
	Uint64	     buttonTime	 = renderStart;

	while (editorState.running_) {

		const Uint64 end = SDL_GetPerformanceCounter();
		const double renderTime =
		    static_cast<double>(end - renderStart) / static_cast<double>(freq);
		const double temp =
		    static_cast<double>(end - buttonTime) / static_cast<double>(freq);

		editor.HandleKeyboardInput();

		if (renderTime >= ticksPerFrame) {
			renderStart = end;
			renderer.Render();
		}

		if (temp * 1250 >= config.editor_.cursor_blink_s) {
			editor.files_.files_.at(0).cursor_.setVisible(
			    !editor.files_.files_.at(0).cursor_.isVisible());
			buttonTime = end;
		}
	}
}