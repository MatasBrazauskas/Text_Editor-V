#ifndef TEXT_EDITOR_SDLWRAPPER_HPP
#define TEXT_EDITOR_SDLWRAPPER_HPP

#include <SDL2/SDL.h>

namespace Wrapper {
    class SdlWindow {
    public:
        SdlWindow();
        ~SdlWindow() noexcept;
    private:
        SDL_Window *window_ = nullptr;
    };

    void Run(size_t seconds);
}

#endif