#include "core/EditorCore.hpp"
#include "core/Renderer.hpp"
#include "utils/ConfigAndSettings.hpp"

int main(const int argc, char** argv) {
	Config config{};
	Settings settings{config};

	EditorCore editorCore{argc, argv, settings};
	Renderer renderer{config, settings};

	const Uint64 freq = SDL_GetPerformanceFrequency();
	Uint64 renderStart = SDL_GetPerformanceCounter();

	while (editorCore.Running()) {

		const Uint64 end = SDL_GetPerformanceCounter();
		const double renderTime = static_cast<double>(end - renderStart) / static_cast<double>(freq);

		editorCore.HandleKeyboardInput();

		if (renderTime >= settings.windowSettings.ticksPerFrame) {
			renderStart = end;


			const LayoutManager layouts{editorCore, config, settings};
			renderer.Render(layouts);
		}
	}
}
