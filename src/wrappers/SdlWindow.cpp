#include "SdlWindow.hpp"
#include <stdexcept>

Wrapper::SdlWindow::SdlWindow(const char* title) : window_(nullptr) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        throw std::runtime_error(SDL_GetError());
    }

    window_ = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        800, 600,
        SDL_WINDOW_SHOWN
    );

    if (!window_) {
        SDL_Quit();
        throw std::runtime_error(SDL_GetError());
    }
}

Wrapper::SdlWindow::~SdlWindow() noexcept {
    if (window_) {
        SDL_DestroyWindow(window_);
    }
    SDL_Quit();
}

bool Wrapper::WindowOpen() {
    SDL_Event e;
    if (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            return false;
        }
    }
    return true;
}
