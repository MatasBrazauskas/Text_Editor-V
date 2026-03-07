#include "core/EditorCore.hpp"
#include "core/Renderer.hpp"
#include "utils/ConfigAndSettings.hpp"

int main(const int argc, char** argv) {

	Config config{};
	Settings settings{config};

	const auto ticksPerFrame = 1.0 / static_cast<double>(config.window.fps_limit);

	EditorCore editorCore{argc, argv, settings};
	Renderer renderer{config, settings};

	const Uint64 freq = SDL_GetPerformanceFrequency();
	Uint64 renderStart = SDL_GetPerformanceCounter();

	while (editorCore.getEditorState().running_) {

		const Uint64 end = SDL_GetPerformanceCounter();
		const double renderTime = static_cast<double>(end - renderStart) / static_cast<double>(freq);

		editorCore.HandleKeyboardInput();

		if (renderTime >= ticksPerFrame) {
			renderStart = end;

			if (editorCore.dirty) {
				const LayoutManager layouts{editorCore, config, settings};
				renderer.Render(layouts);
			}
		}
	}
}
