#include "Renderer.hpp"

Renderer::Renderer(const EditorState& editorState, const Files& files, const Config& config)
	: width_{}, height_{}, editorState_{editorState}, files_{files}, config_{config} {

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
		throw std::runtime_error(SDL_GetError());
	}

	window_ = SDL_CreateWindow(
			config.editor_.title.c_str(),
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			1000, 800,
			SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE //| SDL_WINDOW_BORDERLESS
			);

	if (!window_) {
		SDL_Quit();
		throw std::runtime_error(SDL_GetError());
	}

	renderer_ = SDL_CreateRenderer(window_, -1,
				       SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	if (!renderer_) {
		SDL_Quit();
		throw std::runtime_error(SDL_GetError());
	}

	SDL_StartTextInput();

	if (TTF_Init() == -1) {
		throw std::runtime_error("Failed to initialize TTF.");
	}

	font_ = TTF_OpenFont(config.font_.font_path.c_str(),
			     static_cast<int>(config.font_.font_size));

	if (!font_) {
		throw std::runtime_error("Failed to open font.");
	}

	TTF_SetFontHinting(font_, TTF_HINTING_LIGHT);
	TTF_SizeText(font_, "a", &width_, &height_);
}

Renderer::~Renderer() {
	if (window_ != nullptr) {
		SDL_DestroyWindow(window_);
	}

	if (font_ != nullptr) {
		TTF_CloseFont(font_);
		font_ = nullptr;
	}
}

void Renderer::Render() const {
	const auto bg       = config_.colors_.background_color;
	const auto fg       = config_.colors_.foreground_color;
	const auto cursorBg = config_.colors_.cursor_color;
	const auto cursorFg = config_.colors_.selection_color;
	const auto& [textBuffer, cursor] = files_.getDocument(editorState_.activeTab_);

	SDL_SetRenderDrawColor(renderer_, bg.r, bg.g, bg.b, bg.a);
	SDL_RenderClear(renderer_);

	for (size_t y = 0; y < textBuffer->size(); ++y) {
		const auto line = textBuffer->rowView(y);

		for (size_t x = 0; x < line.size(); ++x) {
			char c[2] = {line[x], '\0'};

			SDL_Surface* surface = TTF_RenderText_Blended(font_, c, fg);

			SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer_, surface);

			SDL_Rect dst{
					static_cast<int>(x * width_),
					static_cast<int>(y * height_),
					surface->w,
					surface->h
			};

			SDL_RenderCopy(renderer_, texture, nullptr, &dst);

			SDL_FreeSurface(surface);
			SDL_DestroyTexture(texture);
		}
	}

	if (cursor.visible_) {
		SDL_SetRenderDrawColor(renderer_,cursorBg.r, cursorBg.g, cursorBg.b, cursorBg.a);

		const SDL_Rect cursorRect{
				static_cast<int>(cursor.x_ * width_),
				static_cast<int>(cursor.y_ * height_),
				width_,
				height_
		};

		SDL_RenderFillRect(renderer_, &cursorRect);

		char ch = ' ';
		if (cursor.y_ < textBuffer->size()) {
			const auto line = textBuffer->rowView(cursor.y_);
			if (cursor.x_ < line.size())
				ch = line[cursor.x_];
		}

		char text[2] = {ch, '\0'};

		SDL_Surface* surface = TTF_RenderText_Blended(font_, text, cursorFg);

		SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer_, surface);

		const SDL_Rect dst{
				static_cast<int>(cursor.x_ * width_),
				static_cast<int>(cursor.y_ * height_),
				surface->w,
				surface->h
		};

		SDL_RenderCopy(renderer_, texture, nullptr, &dst);

		SDL_FreeSurface(surface);
		SDL_DestroyTexture(texture);
	}

	SDL_RenderPresent(renderer_);
}