#include "Renderer.hpp"
#include <SDL_ttf.h>

Renderer::Renderer(ITextBuffer& textBuffer, const Cursor& cursor, const Config& config, Sdl& sdl)
	: width{}, height{}, textBuffer_(textBuffer), cursor_(cursor), config_(config), sdl_(sdl) {

	TTF_SizeText(sdl_.font_, "a", &width, &height);
}

void Renderer::Render() const {
	const auto bg       = config_.colors_.background_color;
	const auto fg       = config_.colors_.foreground_color;
	const auto cursorBg = config_.colors_.cursor_color;
	const auto cursorFg = config_.colors_.selection_color;

	SDL_SetRenderDrawColor(sdl_.renderer_, bg.r, bg.g, bg.b, bg.a);
	SDL_RenderClear(sdl_.renderer_);

	for (size_t y = 0; y < textBuffer_.size(); ++y) {
		const auto line = textBuffer_.rowView(y);

		for (size_t x = 0; x < line.size(); ++x) {
			if (x == cursor_.x_ && y == cursor_.y_)
				continue;

			char c[2] = {line[x], '\0'};

			SDL_Surface* surface =
					TTF_RenderText_Blended(sdl_.font_, c, fg);

			SDL_Texture* texture =
					SDL_CreateTextureFromSurface(sdl_.renderer_, surface);

			SDL_Rect dst{
					static_cast<int>(x * width),
					static_cast<int>(y * height),
					surface->w,
					surface->h
			};

			SDL_RenderCopy(sdl_.renderer_, texture, nullptr, &dst);

			SDL_FreeSurface(surface);
			SDL_DestroyTexture(texture);
		}
	}

	if (cursor_.visible_) {
		SDL_SetRenderDrawColor(
				sdl_.renderer_,
				cursorBg.r, cursorBg.g, cursorBg.b, cursorBg.a
				);

		const SDL_Rect cursorRect{
				static_cast<int>(cursor_.x_ * width),
				static_cast<int>(cursor_.y_ * height),
				width,
				height
		};

		SDL_RenderFillRect(sdl_.renderer_, &cursorRect);

		/* 4. Re-render cursor character */
		char ch = ' ';
		if (cursor_.y_ < textBuffer_.size()) {
			const auto line = textBuffer_.rowView(cursor_.y_);
			if (cursor_.x_ < line.size())
				ch = line[cursor_.x_];
		}

		char text[2] = {ch, '\0'};

		SDL_Surface* surface =
				TTF_RenderText_Blended(sdl_.font_, text, cursorFg);

		SDL_Texture* texture =
				SDL_CreateTextureFromSurface(sdl_.renderer_, surface);

		SDL_Rect dst{
				static_cast<int>(cursor_.x_ * width),
				static_cast<int>(cursor_.y_ * height),
				surface->w,
				surface->h
		};

		SDL_RenderCopy(sdl_.renderer_, texture, nullptr, &dst);

		SDL_FreeSurface(surface);
		SDL_DestroyTexture(texture);
	}

	SDL_RenderPresent(sdl_.renderer_);
}