#include "Renderer.hpp"

RenderWindow::RenderWindow(const Document &doc_t, const int offsetX_t, const int offsetY_t, const int width_t, const int height_t)
    :doc_{doc_t}, offsetX_{offsetX_t}, offsetY_{offsetY_t}, width_{width_t}, height_{height_t} {}

void RenderWindow::Render(const Config& config, SDL_Renderer& renderer, TTF_Font& font, const Modes mode) const {
    RenderText(config, renderer, font);
    RenderCursor(config, renderer, font, mode);
}

void RenderWindow::RenderText(const Config& config, SDL_Renderer& renderer, TTF_Font& font) const {
	/*const auto& [br, bg, bb, ba] = config.colors_.background_color;

	SDL_SetRenderDrawColor(&renderer, br, bg, bb, ba);
    SDL_RenderClear(&renderer);*/

	const auto& [text, view, cursor, _] = doc_;

	int offsetIndex = 0;
	size_t countLimit = std::min(view.visibleLines_, text->linesCount());
	for (const auto it = text->forwardIterator(view.startY_); !it->end(view.startY_ + countLimit); it->next()) {
		auto line = it.get()->getLine();

		if (line.empty() || line.size() <= view.startX_)
			continue;

		if (config.editor_.wrap_text == false) {
			if (view.startX_ + view.visibleColumns_ >= line.size()) {
				line = line.substr(view.startX_);
			} else {
				line = line.substr(view.startX_, view.visibleColumns_);
			}
		} else {
			int startIndex = 0;

			while (startIndex * view.visibleColumns_ < line.size()) {
				const auto subStrLine =
				    line.substr(startIndex * view.visibleColumns_, view.visibleColumns_);
				RenderLine(config, renderer, font, subStrLine, startIndex + offsetIndex + it.get()->index_ - view.startY_);
				startIndex++;
			}

			offsetIndex += startIndex - 1;
			countLimit = std::min(view.visibleLines_ - offsetIndex, text->linesCount());

			continue;
		}

		if (line.empty())
			continue;

		this->RenderLine(config, renderer, font, line, it.get()->index_ - view.startY_ + offsetIndex);
	}
}

void RenderWindow::RenderLine(const Config& config, SDL_Renderer& renderer, TTF_Font& font, const std::string_view line, const int lineOffset) const {
	const auto& fg = config.colors_.foreground_color;

	SDL_Surface* surface = TTF_RenderText_Blended(&font, std::string(line).c_str(), fg);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(&renderer, surface);

	const int length = line.length() * charWidth;

	const SDL_Rect src{0, 0, length, surface->h};
	const SDL_Rect dst{0, lineOffset * charHeight, length, surface->h};

	SDL_RenderCopy(&renderer, texture, &src, &dst);

	SDL_FreeSurface(surface);
	SDL_DestroyTexture(texture);
}

void RenderWindow::RenderCursor(const Config& config, SDL_Renderer& renderer, TTF_Font& font, const Modes mode) const {

	const auto [cr, cg, cb, ca] = config.colors_.cursor_color;
	const auto& cursorFg = config.colors_.selection_color;
	auto& [textBuffer, view, cursor, _] = doc_;

	SDL_SetRenderDrawColor(&renderer, cr, cg, cb, ca);

	int cursorOffsetY = cursor.getY() * charHeight;
	int cursorOffsetX = cursor.getX() * charWidth;

	if (!config.editor_.wrap_text) {
		cursorOffsetX -= view.startX_ * charWidth;
		cursorOffsetY -= view.startY_ * charHeight;
	}

	if (cursor.isVisible()) {
		if (mode == Modes::Insert) {
			const auto rect = SDL_Rect{cursorOffsetX, cursorOffsetY, 1, charHeight};

			SDL_RenderFillRect(&renderer, &rect);
		} else {
			const SDL_Rect cursorRect{cursorOffsetX, cursorOffsetY, charWidth, charHeight};
			SDL_RenderFillRect(&renderer, &cursorRect);

			char ch = ' ';
			if (cursor.getY() < textBuffer->linesCount()) {
				const auto line = textBuffer->rowsView(cursor.getY());
				if (cursor.getX() < line.size())
					ch = line[cursor.getX()];
			}

			const char text[2] = {ch, '\0'};

			SDL_Surface* surface = TTF_RenderText_Blended(&font, text, cursorFg);
			SDL_Texture* texture = SDL_CreateTextureFromSurface(&renderer, surface);

			const SDL_Rect dst{cursorOffsetX, cursorOffsetY, surface->w, surface->h};
			SDL_RenderCopy(&renderer, texture, nullptr, &dst);

			SDL_FreeSurface(surface);
			SDL_DestroyTexture(texture);
		}
	}
}

RenderScreen::RenderScreen(const EditorState& editorState_t, Files& files_t, const Config& config_t)
    : windowWidth_{}, windowHeight_{}, editorState_{editorState_t}, files_{files_t},
      config_{config_t}, tabOffsetX{}, tabOffsetY{} {
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
		throw std::runtime_error(SDL_GetError());
	}

	window_ =
	    SDL_CreateWindow(config_.editor_.title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1000, 800,
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

	if (TTF_Init() == -1) {
		throw std::runtime_error("Failed to initialize TTF.");
	}

	font_ = TTF_OpenFont(config_.font_.font_path.c_str(), config_.font_.font_size);

	if (!font_) {
		throw std::runtime_error("Failed to open font.");
	}

	TTF_SetFontHinting(font_, TTF_HINTING_MONO);
	TTF_SetFontKerning(font_, 0);

	TTF_SizeText(font_, "A", &charWidth, &charHeight);
}

RenderScreen::~RenderScreen() noexcept {
	if (window_ != nullptr) {
		SDL_DestroyWindow(window_);
	}

	if (font_ != nullptr) {
		TTF_CloseFont(font_);
		font_ = nullptr;
	}
}

void RenderScreen::RenderTabs() const {
    const auto& [br, bg, bb, ba] = config_.colors_.background_color;

    SDL_SetRenderDrawColor(renderer_, br, bg, bb, ba);
    SDL_RenderClear(renderer_);

    for (const auto& file : files_.files_) {

    }
}

void RenderScreen::RenderCommandLine() const {
	std::string line{"Normal"};
	SDL_Color bg = {137, 180, 250};

	switch (editorState_.currentMode_) {
	case Modes::Normal: break;
	case Modes::Insert:
		line = "Insert";
		bg = SDL_Color{195, 232, 141};
		break;
	case Modes::Command:
		line = "Command";
		bg = {254, 198, 118};
		break;
	}
	const auto topLRect = SDL_Rect{0, 800 - charHeight - charHeight, windowWidth_, charHeight};

	SDL_SetRenderDrawColor(renderer_, bg.r, bg.g, bg.b, bg.a);
	SDL_RenderFillRect(renderer_, &topLRect);

	SDL_Surface* surface = TTF_RenderText_Blended(font_, std::string(line).c_str(), config_.colors_.selection_color);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer_, surface);

	const SDL_Rect dst{0, 800 - charHeight - charHeight, surface->w, surface->h};
	SDL_RenderCopy(renderer_, texture, nullptr, &dst);

	if (editorState_.currentMode_ == Modes::Command) {
		surface = TTF_RenderText_Blended(font_, editorState_.input_.c_str(), config_.colors_.cursor_color);

		if (surface) {
			texture = SDL_CreateTextureFromSurface(renderer_, surface);

			const SDL_Rect dst2{0, 800 - charHeight, surface->w, surface->h};

			SDL_RenderCopy(renderer_, texture, nullptr, &dst2);
		}
	}

	SDL_FreeSurface(surface);
	SDL_DestroyTexture(texture);
}

void RenderScreen::Render() {
	SDL_GetWindowSize(window_, &windowWidth_, &windowHeight_);

    RenderTabs();

    for (const auto& win : windows_) {
        win.Render(config_, *renderer_, *font_, editorState_.currentMode_);
    }

	RenderCommandLine();

	SDL_RenderPresent(renderer_);
}

void RenderScreen::addWindow(const RenderWindow & window) {
    windows_.push_back(window);
}

void RenderScreen::removeWindow(const RenderWindow & window) {
}
