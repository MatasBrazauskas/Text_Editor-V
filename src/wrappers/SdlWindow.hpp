#pragma once

#include <SDL2/SDL.h>

namespace Wrapper {
    class SdlWindow {
    public:
        SdlWindow(const char* title);
        SdlWindow() = delete;
        ~SdlWindow() noexcept;
    private:
        SDL_Window *window_;
    };

    bool WindowOpen();
}