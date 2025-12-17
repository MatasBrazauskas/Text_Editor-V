#include "SdlWrapper.hpp"
#include <stdexcept>

Wrapper::SdlWrapper::SdlWrapper() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        throw std::runtime_error(SDL_GetError());
    }

    window_ = SDL_CreateWindow(
        "TEMP",
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

Wrapper::SdlWrapper::~SdlWrapper() noexcept {
    if (window_) {
        SDL_DestroyWindow(window_);
    }
    SDL_Quit();
}

void Wrapper::Run(size_t seconds) {
    SDL_Delay(seconds * 1000);
}