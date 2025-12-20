#pragma once

//#include "graphics/SdlWindow.hpp"
//#include "graphics/SdlTtf.h"

#include <SDL.h>
#include <SDL_ttf.h>
#include <string_view>

class Sdl {
public:
    Sdl() = delete;
    Sdl(const char* title, const char* fontsPath, const size_t fontSize);
    ~Sdl();
    void RenderLine(std::string_view line, const SDL_Color& color, const size_t rowX);
    //bool WindowOpen();
//private:
    SDL_Window *window_;
    SDL_Renderer *renderer_;
    TTF_Font* font_;
};