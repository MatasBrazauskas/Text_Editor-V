#include "core/EditorCore.hpp"
#include "core/Renderer.hpp"
#include "utils/Config.hpp"

using namespace std::string_literals;

const std::filesystem::path configPath = "config.json";

int main(const int argc, char** argv) {

    Config config{configPath};
	const auto ticksPerFrame = 1.0 / static_cast<double>(config.editor_.fps);

	EditorCore editorCore{argc, argv};
	Renderer renderer{config};

	const Uint64 freq = SDL_GetPerformanceFrequency();
	Uint64 renderStart = SDL_GetPerformanceCounter();

	while (editorCore.getEditorState().running_) {

		const Uint64 end = SDL_GetPerformanceCounter();
		const double renderTime = static_cast<double>(end - renderStart) / static_cast<double>(freq);

		editorCore.HandleKeyboardInput();

		if (renderTime >= ticksPerFrame) {
			renderStart = end;

		    if (true) {
		        const LayoutManager layouts{editorCore, config};
			    renderer.Render(layouts);
		    }
		}
	}
}
