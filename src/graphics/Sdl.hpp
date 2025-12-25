#pragma once

#include <SDL.h>
#include <SDL_ttf.h>
#include <string_view>

#include "utils/Config.hpp"

class Sdl final {
public:
	Sdl() = delete;

	Sdl(const Config&);

	~Sdl();

	void RenderLine(std::string_view line, const SDL_Color&, const size_t rowX);

	//private:
	SDL_Window*   window_;
	SDL_Renderer* renderer_;
	TTF_Font*     font_;
};