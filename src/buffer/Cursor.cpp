#include "Cursor.hpp"
#include <SDL.h>

Cursor::Cursor(const Config& config): state(false), x{}, y{}, config(config) {}

void Cursor::RenderCursor(Sdl& sdl_) const {
    if (!state) {
        return;
    }

    const auto [r, g, b, a]= config.colors_.cursor_color;

    SDL_SetRenderDrawColor(
        sdl_.renderer_,
        r, g, b, a
    );

    const SDL_Rect rect {
        0,
        0,
        static_cast<int>(config.font_.font_size),
        static_cast<int>(config.font_.font_size)
    };

    SDL_RenderFillRect(sdl_.renderer_, &rect);

    /*SDL_SetRenderDrawColor(sdl_.renderer_, r, g, b, a); // Black background
    //SDL_RenderClear(sdl_.renderer_);

    const char* const temp = "a";

    SDL_Surface* surface = TTF_RenderText_Blended(sdl_.font_, temp, crFrColor);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(sdl_.renderer_, surface);

    const SDL_Rect rect = {0,0, surface->w,surface->h};

    SDL_RenderCopy(sdl_.renderer_, texture, nullptr, &rect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);*/
}
