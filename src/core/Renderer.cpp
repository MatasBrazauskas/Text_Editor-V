#include "Renderer.hpp"

#include "EditorCore.hpp"
#include "buffer/PanesAndLayers.hpp"
#include "utils/ConfigAndSettings.hpp"

#include <iostream>
#include <format>

Renderer::Renderer(const Config& t_config, const Settings& t_settings):
	normalModeColor_{137, 180, 250}, insertModeColor_{195, 232, 141}, commandModeColor_{254, 198, 118},
	config_{t_config}, settings_{t_settings}{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
		throw std::runtime_error(SDL_GetError());
	}

	window_ = SDL_CreateWindow(config_.window.title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, config_.window.width, config_.window.height,
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

	/*if (TTF_Init() == -1) {
		throw std::runtime_error("Failed to initialize TTF.");
	}*/

	const auto& codeFont = t_config.fonts.code;
	const auto& uiFont = t_config.fonts.ui;

	codeFont_ = TTF_OpenFont(codeFont.path.c_str(), codeFont.size);
	uiFont_ = TTF_OpenFont(uiFont.path.c_str(), uiFont.size);

	if (!codeFont_ || !uiFont_) {
		throw std::runtime_error("Failed to open font.");
	}
}

Renderer::~Renderer() noexcept {
	if (renderer_) {
		SDL_DestroyRenderer(renderer_);
	}

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

void Renderer::Render(const LayoutManager& t_layoutManage) const {

	SDL_SetRenderDrawColor(renderer_, 255, 255, 255, 125);
	SDL_RenderClear(renderer_);

	RenderTabs(t_layoutManage.tabLayout, t_layoutManage.windowWidth);
	//RenderPanes(t_layoutManage.panesLayout);
	// RenderCursor(t_layoutManage.cursorLayout);
	RenderCommandLine(t_layoutManage.commandLineLayout, t_layoutManage.windowHeight, t_layoutManage.windowWidth);

	SDL_RenderPresent(renderer_);
}

void Renderer::RenderTabs(const TabLayout& t_tabLayout, const int windowWidth) const {
	const auto& [activeTab, tabCapLines, tabs] = t_tabLayout;

	const auto& main = config_.theme.main;
	const auto& secondary = config_.theme.secondary;

	const auto& codeColor = config_.theme.codeText;
	const auto& uiColor = config_.theme.uiText;

	const auto drawForegroundLine = [&](const int yOffset) {
		static const auto& [fr, fg, fb, fa] = config_.theme.foreground;

		const SDL_Rect barRect = {0, yOffset, windowWidth, settings_.tabHeight};
		SDL_SetRenderDrawColor(renderer_, fr, fg, fb, fa);
		SDL_RenderFillRect(renderer_, &barRect);
	};

	const int paddingX = settings_.paddingX;

	int currentX{};
	int currentY{};

	drawForegroundLine(0);

	for (auto i{0zu}; i < tabs.size(); ++i) {
		const auto& filename = std::string{tabs.at(i)};

		int textW{}, textH{};
		TTF_SizeText(uiFont_, filename.c_str(), &textW, &textH);

		if (currentX + textW >= windowWidth) {
			currentX = 0;
			currentY += settings_.tabHeight;

			drawForegroundLine(currentY);
		}

		const int tabWidth = textW + (paddingX * 2);

		SDL_Rect tabRect = {currentX, currentY, tabWidth, settings_.tabHeight};

		const auto& tabBack = i == activeTab ? main : secondary;
		const auto& tabTextColor = i == activeTab ? codeColor : uiColor ;

		SDL_SetRenderDrawColor(renderer_, tabBack.r, tabBack.g, tabBack.b, tabBack.a);
		SDL_RenderFillRect(renderer_, &tabRect);

		SDL_SetRenderDrawColor(renderer_, 20, 20, 20, 255);
		SDL_RenderDrawLine(renderer_, currentX + tabWidth - 1, currentY, currentX + tabWidth - 1, settings_.uiCharHeight+2);


		SDL_Surface* surface = TTF_RenderText_Blended(uiFont_, filename.c_str(), tabTextColor);
		SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer_, surface);

		SDL_Rect textRect = {currentX + paddingX,(settings_.tabHeight - textH) / 2 + currentY, textW, textH};

		SDL_RenderCopy(renderer_, texture, nullptr, &textRect);

		SDL_FreeSurface(surface);
		SDL_DestroyTexture(texture);

		currentX += tabWidth;
	}
}

void Renderer::RenderPanes(const std::vector<PanesLayout>& panes) const {
	const auto& fg = config_.theme.foreground;

	const int codeCharWidth = settings_.codeCharWidth;
	const int codeCharHeight = settings_.codeCharHeight;

	for (const auto& pane : panes) {
		const auto& [startX, startY, endX, endY, leftDataOffsetX, leftData, lines] = pane;

		for (auto i{0zu}; i < lines.size(); i++) {
			const auto& line = std::string{lines.at(i)};

			SDL_Surface* surface = TTF_RenderText_Blended(codeFont_, line.c_str(), fg);
			SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer_, surface);

			const int length = line.length() * codeCharWidth;
			const int lineOffset = i * codeCharHeight;

			const SDL_Rect src{0, 0, length, surface->h};
			const SDL_Rect dst{startX + leftDataOffsetX, startY + lineOffset * codeCharHeight, length, surface->h};

			SDL_RenderCopy(renderer_, texture, &src, &dst);

			SDL_FreeSurface(surface);
			SDL_DestroyTexture(texture);
		}
	}
}

void Renderer::RenderCursor(const CursorLayout& t_cursorLayout) const {
	const auto [cr, cg, cb, ca] = config_.theme.cursor;
	const auto& cursorFg = config_.theme.highlight;

	const auto& [active, cursorX, cursorY, letter, panesLayout] = t_cursorLayout;

	SDL_SetRenderDrawColor(renderer_, cr, cg, cb, ca);
	const auto& [codeCharWidth, codeCharHeight, uiCharWidth, uiCharHeight, tabHeight, paddingX] = settings_;

	const int cursorOffsetY = codeCharHeight;
	const int cursorOffsetX = codeCharWidth;

	if (active) {
		if (letter == '\0') {
			const auto rect = SDL_Rect{cursorOffsetX, cursorOffsetY, 1, codeCharHeight};

			SDL_RenderFillRect(renderer_, &rect);
		} else {
			const SDL_Rect cursorRect{cursorOffsetX, cursorOffsetY, codeCharWidth, codeCharHeight};
			SDL_RenderFillRect(renderer_, &cursorRect);

			const char text[2] = {letter, '\0'};

			SDL_Surface* surface = TTF_RenderText_Blended(codeFont_, text, cursorFg);
			SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer_, surface);

			const SDL_Rect dst{cursorOffsetX, cursorOffsetY, surface->w, surface->h};
			SDL_RenderCopy(renderer_, texture, nullptr, &dst);

			SDL_FreeSurface(surface);
			SDL_DestroyTexture(texture);
		}
	}
}

void Renderer::RenderCommandLine(const CommandLineLayout& t_commandLineLayout, const int windowHeight, const int windowWidth) const {
	const auto& [codeCharWidth, codeCharHeight, uiCharWidth, uiCharHeight, tabHeight, paddingX] = settings_;
	const auto& [mode, currFileName, currCommand, cursorX, cursorY, charCount, lineCount, args] = t_commandLineLayout;

	const auto& foreground = config_.theme.foreground;

	std::string line{"  Normal"};
	SDL_Color bg = normalModeColor_;

	switch (mode) {
	case Modes::Insert:
		line = "  Insert";
		bg = insertModeColor_;
		break;
	case Modes::Command:
		line = "  Command";
		bg = commandModeColor_;
		break;
	}

	const int modeBarWidth = uiCharWidth * 11;
	const std::string cappedFilename = currFileName.substr(0, 20);

	const std::string formattedCommandAndCursorInfo = std::format("Command: {:>2}, ({}, {})",currCommand, cursorX, cursorY);
	const std::string formattedFileInfo = std::format("Lines: {:>2}, Chars: {:>2}", lineCount, charCount);

	const auto topLRect = SDL_Rect{0, windowHeight - (2 * uiCharHeight), windowWidth, 2 * uiCharHeight};
	SDL_SetRenderDrawColor(renderer_, foreground.r, foreground.g, foreground.b, foreground.a);
	SDL_RenderFillRect(renderer_, &topLRect);

	SDL_Surface* surface = TTF_RenderText_Blended(uiFont_, line.c_str(), config_.theme.uiText);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer_, surface);

	const auto modeRect = SDL_Rect{0, windowHeight - (2 * uiCharHeight), modeBarWidth, uiCharHeight};
	SDL_SetRenderDrawColor(renderer_, bg.r, bg.g, bg.b, bg.a);
	SDL_RenderFillRect(renderer_, &modeRect);

	SDL_Rect dst{0, windowHeight - 2 * uiCharHeight, surface->w, surface->h};
	SDL_RenderCopy(renderer_, texture, nullptr, &dst);

	surface = TTF_RenderText_Blended(uiFont_, cappedFilename.c_str(), config_.theme.uiText);
	texture = SDL_CreateTextureFromSurface(renderer_, surface);

	dst = {modeBarWidth, windowHeight - 2 * uiCharHeight, surface->w, surface->h};
	SDL_RenderCopy(renderer_, texture, nullptr, &dst);

	auto surfaceTemp = TTF_RenderText_Blended(uiFont_, formattedFileInfo.c_str(), bg);
	auto textureTemp = SDL_CreateTextureFromSurface(renderer_, surfaceTemp);

	dst = {windowWidth - surfaceTemp->w, windowHeight - 2 * uiCharHeight, surfaceTemp->w, surface->h};
	SDL_RenderCopy(renderer_, textureTemp, nullptr, &dst);

	auto surfaceTemp2 = TTF_RenderText_Blended(uiFont_, formattedCommandAndCursorInfo.c_str(), config_.theme.uiText);
	auto textureTemp2 = SDL_CreateTextureFromSurface(renderer_, surfaceTemp2);

	dst = {windowWidth - surfaceTemp->w - surfaceTemp2->w, windowHeight - 2 * uiCharHeight, surfaceTemp2->w, surfaceTemp2->h};
	SDL_RenderCopy(renderer_, textureTemp2, nullptr, &dst);

	SDL_FreeSurface(surface);
	SDL_DestroyTexture(texture);
}