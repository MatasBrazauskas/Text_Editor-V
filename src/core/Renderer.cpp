#include "Renderer.hpp"

#include <iostream>

RenderWindow::RenderWindow(const std::size_t index_t, const int offsetX_t, const int offsetY_t, const int width_t,
			   const int height_t)
    : index{index_t}, offsetX_{offsetX_t}, offsetY_{offsetY_t}, width_{width_t}, height_{height_t} {}

void RenderWindow::Render(const Document& doc_t, const Config& config, SDL_Renderer& renderer, TTF_Font& font,
			  const Modes mode) const {
	RenderText(doc_t, config, renderer, font);
	RenderCursor(doc_t, config, renderer, font, mode);
}

void RenderWindow::RenderText(const Document& doc_t, const Config& config, SDL_Renderer& renderer,
			      TTF_Font& font) const {
	const auto& [text, view, cursor, _] = doc_t;

	int offsetIndex = 0;
	size_t countLimit = std::min(view.visibleLines_, text->linesCount());

	if (countLimit == 0) {
		return;
	}

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
				RenderLine(config, renderer, font, subStrLine,
					   startIndex + offsetIndex + it.get()->index_ - view.startY_);
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

void RenderWindow::RenderLine(const Config& config, SDL_Renderer& renderer, TTF_Font& font, const std::string_view line,
			      const int lineOffset) const {
	const auto& fg = config.colors_.foreground_color;

	SDL_Surface* surface = TTF_RenderText_Blended(&font, std::string(line).c_str(), fg);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(&renderer, surface);

	const int length = line.length() * codeCharWidth;

	const SDL_Rect src{0, 0, length, surface->h};
	const SDL_Rect dst{0, tabOffsetY + lineOffset * codeCharHeight, length, surface->h};

	SDL_RenderCopy(&renderer, texture, &src, &dst);

	SDL_FreeSurface(surface);
	SDL_DestroyTexture(texture);
}

void RenderWindow::RenderCursor(const Document& doc_t, const Config& config, SDL_Renderer& renderer, TTF_Font& font,
				const Modes mode) const {

	const auto [cr, cg, cb, ca] = config.colors_.cursor_color;
	const auto& cursorFg = config.colors_.selection_color;
	auto& [textBuffer, view, cursor, _] = doc_t;

	SDL_SetRenderDrawColor(&renderer, cr, cg, cb, ca);

	int cursorOffsetY = cursor.getY() * codeCharHeight;
	int cursorOffsetX = cursor.getX() * codeCharWidth;

	if (!config.editor_.wrap_text) {
		cursorOffsetX -= view.startX_ * codeCharWidth;
		cursorOffsetY -= view.startY_ * codeCharHeight;
	}

	if (cursor.isVisible()) {
		if (mode == Modes::Insert) {
			const auto rect = SDL_Rect{cursorOffsetX, tabOffsetY + cursorOffsetY, 1, codeCharHeight};

			SDL_RenderFillRect(&renderer, &rect);
		} else {
			const SDL_Rect cursorRect{cursorOffsetX, tabOffsetY + cursorOffsetY, codeCharWidth,
						  codeCharHeight};
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

			const SDL_Rect dst{cursorOffsetX, tabOffsetY + cursorOffsetY, surface->w, surface->h};
			SDL_RenderCopy(&renderer, texture, nullptr, &dst);

			SDL_FreeSurface(surface);
			SDL_DestroyTexture(texture);
		}
	}
}

RenderScreen::RenderScreen(const EditorState& editorState_t, Files& files_t, const Config& config_t)
    : windowWidth_{}, windowHeight_{}, editorState_{editorState_t}, files_{files_t}, config_{config_t} {
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

	codeFont_ = TTF_OpenFont(config_.font_.code_font_path.c_str(), config_.font_.code_font_size);
	uiFont_ = TTF_OpenFont(config_.font_.ui_font_path.c_str(), config_.font_.ui_font_size);

	if (!codeFont_ || !uiFont_) {
		throw std::runtime_error("Failed to open font.");
	}

	TTF_SetFontHinting(codeFont_, TTF_HINTING_MONO);
	TTF_SetFontKerning(codeFont_, 0);

	TTF_SizeText(codeFont_, "A", &codeCharWidth, &codeCharHeight);

	TTF_SetFontHinting(uiFont_, TTF_HINTING_MONO);
	TTF_SetFontKerning(uiFont_, 0);

	TTF_SizeText(uiFont_, "A", &uiCharWidth, &uiCharHeight);
}

RenderScreen::~RenderScreen() noexcept {
	if (window_ != nullptr) {
		SDL_DestroyWindow(window_);
	}

	if (codeFont_) {
		TTF_CloseFont(codeFont_);
		codeFont_ = nullptr;
	}

	if (uiFont_) {
		TTF_CloseFont(uiFont_);
		uiFont_ = nullptr;
	}
}

void RenderScreen::RenderTabs() {
	const int tabHeight = uiCharHeight + 5;
	const int paddingX = 25;

	constexpr SDL_Color colBarBg = {30, 30, 30, 255};
	constexpr SDL_Color colTabInact = {45, 45, 45, 255};
	constexpr SDL_Color colTabAct = {30, 32, 40, 255};
	constexpr SDL_Color colAccent = {0, 122, 204, 255};
	constexpr SDL_Color colTextAct = {255, 255, 255, 255};
	constexpr SDL_Color colTextInact = {150, 150, 150, 255};

	int currentX{};
	int currentY{};

	const SDL_Rect barRect = {0, 0, windowWidth_, tabHeight};
	SDL_SetRenderDrawColor(renderer_, colBarBg.r, colBarBg.g, colBarBg.b, colBarBg.a);
	SDL_RenderFillRect(renderer_, &barRect);

	for (size_t i = 0; i < files_.files_.size(); ++i) {
		const auto& file = files_.files_[i];
		const auto fileName = file.filesPath_.filename().string();

		int textW{}, textH{};
		TTF_SizeText(uiFont_, fileName.c_str(), &textW, &textH);

		const int tabWidth = textW + (paddingX * 2);
		const bool isActive = i == editorState_.activeTab_;

		if (currentX + tabWidth >= windowWidth_) {
			currentX = 0;
			currentY += tabHeight;

			const SDL_Rect tempRect = {0, currentY, windowWidth_, tabHeight};
			SDL_SetRenderDrawColor(renderer_, colBarBg.r, colBarBg.g, colBarBg.b, colBarBg.a);
			SDL_RenderFillRect(renderer_, &tempRect);
		}

		SDL_Rect tabRect = {currentX, currentY, tabWidth, tabHeight};

		// A. Draw Tab Background
		SDL_Color bg = isActive ? colTabAct : colTabInact;
		SDL_SetRenderDrawColor(renderer_, bg.r, bg.g, bg.b, bg.a);
		SDL_RenderFillRect(renderer_, &tabRect);

		if (isActive) {
			SDL_Rect accentRect = {currentX, currentY, tabWidth, 2}; // 2px height
			SDL_SetRenderDrawColor(renderer_, colAccent.r, colAccent.g, colAccent.b, colAccent.a);
			SDL_RenderFillRect(renderer_, &accentRect);
		}

		SDL_SetRenderDrawColor(renderer_, 20, 20, 20, 255); // Dark line
		SDL_RenderDrawLine(renderer_, currentX + tabWidth - 1, currentY, currentX + tabWidth - 1,
				   currentY + tabHeight);

		SDL_Color textColor = isActive ? colTextAct : colTextInact;

		SDL_Surface* surface = TTF_RenderText_Blended(uiFont_, fileName.c_str(), textColor);
		SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer_, surface);

		SDL_Rect textRect = {currentX + paddingX, (tabHeight - textH) / 2 + currentY, textW, textH};

		SDL_RenderCopy(renderer_, texture, nullptr, &textRect);

		SDL_FreeSurface(surface);
		SDL_DestroyTexture(texture);

		currentX += tabWidth;
	}

	tabOffsetY = currentY;
}

void RenderScreen::RenderCommandLine() const {
	std::string line{"Normal"};
	SDL_Color bg = {137, 180, 250};

	switch (editorState_.currentMode_) {
	case Modes::Normal:
		break;
	case Modes::Insert:
		line = "Insert";
		bg = SDL_Color{195, 232, 141};
		break;
	case Modes::Command:
		line = "Command";
		bg = {254, 198, 118};
		break;
	}
	const auto topLRect = SDL_Rect{0, windowHeight_ - uiCharHeight - uiCharHeight, windowWidth_, uiCharHeight};

	SDL_SetRenderDrawColor(renderer_, bg.r, bg.g, bg.b, bg.a);
	SDL_RenderFillRect(renderer_, &topLRect);

	SDL_Surface* surface =
	    TTF_RenderText_Blended(uiFont_, std::string(line).c_str(), config_.colors_.selection_color);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer_, surface);

	const SDL_Rect dst{0, windowHeight_ - uiCharHeight - uiCharHeight, surface->w, surface->h};
	SDL_RenderCopy(renderer_, texture, nullptr, &dst);

	if (editorState_.currentMode_ == Modes::Command) {
		surface = TTF_RenderText_Blended(uiFont_, editorState_.input_.c_str(), config_.colors_.cursor_color);

		// TODO fix this ugly mess
		const int lineLength = editorState_.input_.length();
		const auto& [r, g, b, a] = config_.colors_.cursor_color;
		SDL_SetRenderDrawColor(renderer_, r, g, b, a);
		const auto rect = SDL_Rect{lineLength * uiCharWidth, windowHeight_ - uiCharHeight, 1, codeCharHeight};

		SDL_RenderFillRect(renderer_, &rect);

		if (surface) {
			texture = SDL_CreateTextureFromSurface(renderer_, surface);

			const SDL_Rect dst2{0, windowHeight_ - uiCharHeight, surface->w, surface->h};

			SDL_RenderCopy(renderer_, texture, nullptr, &dst2);
		}
	}

	SDL_FreeSurface(surface);
	SDL_DestroyTexture(texture);
}

void RenderScreen::Render() {
	SDL_GetWindowSize(window_, &windowWidth_, &windowHeight_);
	const auto& [br, bg, bb, ba] = config_.colors_.background_color;

	SDL_SetRenderDrawColor(renderer_, br, bg, bb, ba);
	SDL_RenderClear(renderer_);

	for (const auto& win : windows_) {
		win.Render(files_.files_.at(win.index), config_, *renderer_, *codeFont_, editorState_.currentMode_);
	}

	RenderTabs();
	RenderCommandLine();

	SDL_RenderPresent(renderer_);
}

void RenderScreen::addWindow(const RenderWindow& window) {
	windows_.push_back(window);
}

void RenderScreen::removeWindow(const RenderWindow& window) {}
