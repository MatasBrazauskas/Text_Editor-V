#include "Sdl.hpp"
#include <stdexcept>

Sdl::Sdl(const char* title, const char* fontsPath, const size_t fontSize) : window_(nullptr), renderer_(nullptr), font_(nullptr) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
        throw std::runtime_error(SDL_GetError());
    }


    window_ = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        800, 600,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE //| SDL_WINDOW_BORDERLESS
    );

    if (!window_) {
        SDL_Quit();
        throw std::runtime_error(SDL_GetError());
    }


    renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (!renderer_) {
        SDL_Quit();
        throw std::runtime_error(SDL_GetError());
    }

    SDL_StartTextInput();
    //SDL_StopTextInput();

    if (TTF_Init() == -1) {
        throw std::runtime_error("Failed to initialize TTF.");
    }

    font_ = TTF_OpenFont(fontsPath, static_cast<int>(fontSize));

    if (!font_) {
        throw std::runtime_error("Failed to open font.");
    }

    TTF_SetFontHinting(font_, TTF_HINTING_LIGHT);
}

Sdl::~Sdl() {
    if (window_) {
        SDL_DestroyWindow(window_);
    }

    if (font_ != nullptr) {
        TTF_CloseFont(font_);
        font_ = nullptr;
    }

    SDL_Quit();
}

void Sdl::RenderLine(std::string_view line, const SDL_Color& color, const size_t rowX) {
    SDL_Surface* surface = TTF_RenderText_Blended(font_, line.data(), color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer_, surface);

    SDL_Rect rect = {0,static_cast<int>(rowX), surface->w,surface->h};

    SDL_RenderCopy(renderer_, texture, nullptr, &rect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}