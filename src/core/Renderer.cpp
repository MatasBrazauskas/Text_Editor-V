#include "Renderer.hpp"

Renderer::Renderer(const EditorState& editorState, const Files& files, const Config& config)
	: charWidth_{}, charHeight_{}, windowWidth_{}, windowHeight_{}, editorState_{editorState}, files_{files}, config_{config} {

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
	TTF_SizeText(font_, "a", &charWidth_, &charHeight_);
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

void Renderer::RenderText() const {
	const auto bg = config_.colors_.background_color;
	const auto fg = config_.colors_.foreground_color;

	const auto& [textBuffer, view, cursor, _] = files_.getDocument(editorState_.activeTab_);

	SDL_SetRenderDrawColor(renderer_, bg.r, bg.g, bg.b, bg.a);
	SDL_RenderClear(renderer_);

	for (auto y{0}; y < std::min(view.visibleLines_, textBuffer->linesCount()); ++y) {
		auto line = textBuffer->rowsView(y + view.startY_);

		if (line.empty() || line.size() <= view.startX_) continue;

	    if (config_.editor_.wrap_text) {
	        if (view.startX_ + view.visibleColumns_ >= line.size()) {
	            line = line.substr(view.startX_);
	        } else {
	            line = line.substr(view.startX_, view.visibleColumns_);
	        }
	    } else {
	        line = line.substr(0, view.visibleColumns_);
	    }

		if (line.empty()) continue;

		SDL_Surface* surface = TTF_RenderText_Blended(font_, std::string(line).c_str(), fg);
		SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer_, surface);

		SDL_Rect dst {
			0,
			y * charHeight_,
			surface->w,
			surface->h
		    };

		SDL_RenderCopy(renderer_, texture, nullptr, &dst);

		SDL_FreeSurface(surface);
		SDL_DestroyTexture(texture);
	}
}

void Renderer::RenderCursor() const {

	const auto cursorBg = config_.colors_.cursor_color;
	const auto cursorFg = config_.colors_.selection_color;
	const auto& [textBuffer, view, cursor, _] = files_.getDocument(editorState_.activeTab_);

    SDL_SetRenderDrawColor(renderer_,cursorBg.r, cursorBg.g, cursorBg.b, cursorBg.a);

    int cursorOffsetY = cursor.getY() * charHeight_;
    int cursorOffsetX = cursor.getX() * charWidth_;

    if (config_.editor_.wrap_text) {
        cursorOffsetX -= view.startX_ * charWidth_;
        cursorOffsetY -= view.startY_ * charHeight_;
    }

	if (cursor.isVisible()) {
	    if (editorState_.currentMode_ == Modes::Insert) {
	        const auto rect = SDL_Rect{cursorOffsetX, cursorOffsetY, 1, charHeight_};
	        SDL_RenderFillRect(renderer_, &rect);
	    }else {
	        const SDL_Rect cursorRect{cursorOffsetX, cursorOffsetY, charWidth_, charHeight_};
	        SDL_RenderFillRect(renderer_, &cursorRect);

	        char ch = ' ';
	        if (cursor.getY() < textBuffer->linesCount()) {
	            const auto line = textBuffer->rowsView(cursor.getY());
	            if (cursor.getX() < line.size())
	                ch = line[cursor.getX()];
	        }

	        const char text[2] = {ch, '\0'};

	        SDL_Surface* surface = TTF_RenderText_Blended(font_, text, cursorFg);
	        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer_, surface);

	        const SDL_Rect dst{cursorOffsetX, cursorOffsetY, surface->w, surface->h};
	        SDL_RenderCopy(renderer_, texture, nullptr, &dst);

	        SDL_FreeSurface(surface);
	        SDL_DestroyTexture(texture);
	    }
	}
}

void Renderer::RenderCommandLine() const {
    const auto topLRect = SDL_Rect{0, 800 - charHeight_ - charHeight_, windowWidth_, charHeight_};
    SDL_SetRenderDrawColor(renderer_, config_.colors_.cursor_color.r, config_.colors_.cursor_color.g, config_.colors_.cursor_color.b, config_.colors_.cursor_color.a);
    SDL_RenderFillRect(renderer_, &topLRect);

    std::string line{};

    switch (editorState_.currentMode_) {
        case Modes::Normal: line = "Normal"; break;
        case Modes::Insert: line = "Insert"; break;
        case Modes::Command: line = "Command"; break;
    }

    SDL_Surface* surface = TTF_RenderText_Blended(font_, std::string(line).c_str(), config_.colors_.selection_color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer_, surface);

    SDL_Rect dst {0,800 - charHeight_ - charHeight_,surface->w,surface->h};
    SDL_RenderCopy(renderer_, texture, nullptr, &dst);


    if (editorState_.currentMode_ == Modes::Command) {
        surface = TTF_RenderText_Blended(font_, editorState_.input_.c_str(), config_.colors_.cursor_color);
        texture = SDL_CreateTextureFromSurface(renderer_, surface);

        SDL_Rect dst {0, 800 - charHeight_, surface->w,surface->h};

        SDL_RenderCopy(renderer_, texture, nullptr, &dst);
    }

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void Renderer::Render() {
	SDL_GetWindowSize(window_, &windowWidth_, &windowHeight_);

	RenderText();
	RenderCursor();
	RenderCommandLine();

	SDL_RenderPresent(renderer_);
}

