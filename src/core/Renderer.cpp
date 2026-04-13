#include "Renderer.hpp"

#include "EditorCore.hpp"
#include "buffer/PanesAndLayers.hpp"
#include "utils/ConfigAndSettings.hpp"

#include <format>
#include <iostream>

Renderer::Renderer(const Config& t_config, const Settings& t_settings) : config_{t_config}, settings_{t_settings} {
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
		throw std::runtime_error(SDL_GetError());
	}

	window_ = SDL_CreateWindow(config_.window.title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
							   config_.window.width, config_.window.height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

	SDL_SetWindowMinimumSize(window_, t_config.window.minWidth, t_config.window.minHeight);

	if (!window_) {
		SDL_Quit();
		throw std::runtime_error(SDL_GetError());
	}

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	if (!renderer_) {
		SDL_Quit();
		throw std::runtime_error(SDL_GetError());
	}

	SDL_StartTextInput();

	if (TTF_Init() != 0) {
		throw std::runtime_error("Failed to initialize TTF.");
	}

	const auto& codeFont = t_config.fonts.code;
	const auto& uiFont = t_config.fonts.ui;

	codeFont_ = TTF_OpenFont(codeFont.path.c_str(), codeFont.size);
	uiFont_ = TTF_OpenFont(uiFont.path.c_str(), uiFont.size);

	if (!codeFont_ || !uiFont_) {
		throw std::runtime_error("Failed to open font.");
	}

	colorMap_ = {
		{Modes::Normal, {137, 180, 250}},
		{Modes::Insert, {195, 232, 141}},
		{Modes::Command, {254, 198, 118}},
		{Modes::WindowMode, {203, 166, 247}},
		{Modes::FileMode, {245, 194, 231}},
	};
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
	SDL_RenderClear(renderer_);

	RenderTabs(t_layoutManage.tabLayout);
	RenderPanes(t_layoutManage.panesLayout);
	RenderCursor(t_layoutManage.cursorLayout);
	RenderCommandLine(t_layoutManage.commandLineLayout);

	SDL_RenderPresent(renderer_);
}

void Renderer::RenderTabs(const TabLayout& t_tabLayout) const {
	const auto& [activeTab, tabCapLines, tabs] = t_tabLayout;
	const auto& [charStg, winStg] = settings_;

	const auto& main = config_.theme.main;
	const auto& secondary = config_.theme.secondary;

	const auto& codeColor = config_.theme.codeText;
	const auto& uiColor = config_.theme.uiText;

	const auto drawForegroundLine = [&](const int yOffset) {
		static const auto& [fr, fg, fb, fa] = config_.theme.foreground;

		const SDL_Rect barRect = {0, yOffset, winStg.width, charStg.tabHeight};
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

		if (currentX + textW >= winStg.width) {
			currentX = 0;
			currentY += charStg.tabHeight;

			drawForegroundLine(currentY);
		}

		const int tabWidth = textW + (paddingX * 2);

		SDL_Rect tabRect = {currentX, currentY, tabWidth, charStg.tabHeight};

		const auto& tabBack = i == activeTab ? main : secondary;
		const auto& tabTextColor = i == activeTab ? codeColor : uiColor;

		SDL_SetRenderDrawColor(renderer_, tabBack.r, tabBack.g, tabBack.b, tabBack.a);
		SDL_RenderFillRect(renderer_, &tabRect);

		SDL_SetRenderDrawColor(renderer_, 20, 20, 20, 255);
		SDL_RenderDrawLine(renderer_, currentX + tabWidth - 1, currentY, currentX + tabWidth - 1,
						   charStg.uiCharHeight + 2);

		SDL_Surface* surface = TTF_RenderText_Blended(uiFont_, filename.c_str(), tabTextColor);
		SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer_, surface);

		SDL_Rect textRect = {currentX + paddingX, (charStg.tabHeight - textH) / 2 + currentY, textW, textH};

		SDL_RenderCopy(renderer_, texture, nullptr, &textRect);

		SDL_FreeSurface(surface);
		SDL_DestroyTexture(texture);

		currentX += tabWidth;
	}
}

void Renderer::RenderRegularFile(const PanesLayout& t_pane) const {
	const auto& codeColor = config_.theme.codeText;
	const auto& background = config_.theme.background;
	const auto& foregound = config_.theme.foreground;
	const auto& [charStg, winStg] = settings_;

	const auto& [paneSnippet, startX, startY, endX, endY, leftDataOffsetX, leftData, lines] = t_pane;

	const SDL_Rect box = {startX, startY, endX - startX, endY - startY};

	SDL_SetRenderDrawColor(renderer_, background.r, background.g, background.b, background.a);
	SDL_RenderFillRect(renderer_, &box);

	const SDL_Rect leftSideBox = {startX, startY, leftDataOffsetX, endY - startY};
	SDL_SetRenderDrawColor(renderer_, foregound.r, foregound.g, foregound.b, foregound.a);
	SDL_RenderFillRect(renderer_, &leftSideBox);

	SDL_SetRenderDrawColor(renderer_, codeColor.r, codeColor.g, codeColor.b, codeColor.a);
	SDL_RenderDrawRect(renderer_, &box);

	for (auto i{0zu}; i < lines.size(); i++) {
		std::string line = std::string{lines.at(i)};
		line.insert(0, leftData.at(i));

		SDL_Surface* surface = TTF_RenderText_Blended(codeFont_, line.c_str(), codeColor);
		if (!surface)
			continue;

		SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer_, surface);

		const int lineOffset = i * charStg.codeCharHeight;
		const SDL_Rect dst{startX, startY + lineOffset, surface->w, surface->h};

		SDL_RenderCopy(renderer_, texture, nullptr, &dst);

		SDL_FreeSurface(surface);
		SDL_DestroyTexture(texture);
	}
}

void Renderer::RenderFileExplorer(const PanesLayout& t_pane) const {
	const auto& codeColor = config_.theme.codeText;
	const auto& background = config_.theme.background;
	const auto& foregound = config_.theme.foreground;
	const auto& [charStg, winStg] = settings_;

	const auto& [paneSnippet, startX, startY, endX, endY, leftDataOffsetX, leftData, lines] = t_pane;

	const SDL_Rect box = {startX, startY, endX - startX, endY - startY};

	SDL_SetRenderDrawColor(renderer_, background.r, background.g, background.b, background.a);
	SDL_RenderFillRect(renderer_, &box);

	const SDL_Rect leftSideBox = {startX, startY, leftDataOffsetX, endY - startY};
	SDL_SetRenderDrawColor(renderer_, foregound.r, foregound.g, foregound.b, foregound.a);
	SDL_RenderFillRect(renderer_, &leftSideBox);

	SDL_SetRenderDrawColor(renderer_, codeColor.r, codeColor.g, codeColor.b, codeColor.a);
	SDL_RenderDrawRect(renderer_, &box);

	for (auto i{0zu}; i < lines.size(); i++) {
		const auto line = lines.at(i);

		SDL_Surface* surface = TTF_RenderText_Blended(codeFont_, line.c_str(), codeColor);
		if (!surface)
			continue;

		SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer_, surface);

		const int lineOffset = i * charStg.codeCharHeight;
		const SDL_Rect dst{startX, startY + lineOffset, surface->w, surface->h};

		SDL_RenderCopy(renderer_, texture, nullptr, &dst);

		SDL_FreeSurface(surface);
		SDL_DestroyTexture(texture);
	}
}

void Renderer::RenderPanes(const std::vector<PanesLayout>& panes) const {

	for (const auto& pane : panes) {
		if (pane.panesSnippet == PanesSnippets::TextSnippet) {
			RenderRegularFile(pane);
		} else {
			RenderFileExplorer(pane);
		}
	}
}

void Renderer::RenderCursor(const CursorLayout& t_cursorLayout) const {
	const auto [cr, cg, cb, ca] = config_.theme.cursor;
	const auto& cursorFg = config_.theme.highlight;

	const auto& [active, cursorX, cursorY, letters, cursorWidth, cursorType] = t_cursorLayout;

	SDL_SetRenderDrawColor(renderer_, cr, cg, cb, ca);
	const auto& [charStg, winStg] = settings_;

	if (active) {
		const SDL_Rect cursorRect{cursorX, cursorY, cursorWidth, charStg.codeCharHeight};
		SDL_RenderFillRect(renderer_, &cursorRect);

		SDL_Surface* surface = TTF_RenderText_Blended(codeFont_, letters.c_str(), cursorFg);
		SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer_, surface);

		if (surface) {
			const SDL_Rect dst{cursorX, cursorY, surface->w, surface->h};
			SDL_RenderCopy(renderer_, texture, nullptr, &dst);

			SDL_FreeSurface(surface);
			SDL_DestroyTexture(texture);
		}
	}
}

void Renderer::RenderCommandLine(const CommandLineLayout& t_layout) const {
	const auto& [charStg, winStg] = settings_;
	const auto [w, h, tickRate] = winStg;
	const auto& [mode, modeName, inputInfo, fileInfo, commandLineState, cursorIndexX, commandLineInfo] = t_layout;

	const SDL_Color modeBg = colorMap_.at(mode);
	const int barY = h - (2 * charStg.uiCharHeight);

	const SDL_Rect barRect = {0, barY, w, 2 * charStg.uiCharHeight};
	SDL_SetRenderDrawColor(renderer_, config_.theme.foreground.r, config_.theme.foreground.g, config_.theme.foreground.b, 255);
	SDL_RenderFillRect(renderer_, &barRect);

	const int modeBarWidth = charStg.uiCharWidth * 11;
	const SDL_Rect modeRect = {0, barY, modeBarWidth, charStg.uiCharHeight};
	SDL_SetRenderDrawColor(renderer_, modeBg.r, modeBg.g, modeBg.b, 255);
	SDL_RenderFillRect(renderer_, &modeRect);

	SDL_Surface* s = TTF_RenderText_Blended(uiFont_, modeName.c_str(), config_.theme.uiText);
	SDL_Texture* t = SDL_CreateTextureFromSurface(renderer_, s);

	SDL_Rect dst = {0, barY, s->w, s->h};
	SDL_RenderCopy(renderer_, t, nullptr, &dst);

	SDL_FreeSurface(s);
	SDL_DestroyTexture(t);

	auto* input = TTF_RenderText_Blended(uiFont_, inputInfo.c_str(), config_.theme.uiText);
	if (input) {
		auto* textureInput = SDL_CreateTextureFromSurface(renderer_, input);

		dst = {modeBarWidth + 5, barY, input->w, input->h};
		SDL_RenderCopy(renderer_, textureInput, nullptr, &dst);

		SDL_FreeSurface(input);
		SDL_DestroyTexture(textureInput);
	}


	SDL_SetRenderDrawColor(renderer_, modeBg.r, modeBg.g, modeBg.b, 255);
	auto* file = TTF_RenderText_Blended(uiFont_, fileInfo.c_str(), config_.theme.uiText);
	if (file) {
		auto* textureInput = SDL_CreateTextureFromSurface(renderer_, file);

		dst = {w - file->w - 5, barY, file->w, file->h};
		SDL_RenderCopy(renderer_, textureInput, nullptr, &dst);

		SDL_FreeSurface(file);
		SDL_DestroyTexture(textureInput);
	}

	/*drawText(modeName, config_.theme.uiText, 0, barY);
	drawText(commandInfo, config_.theme.uiText, modeBarWidth + 5, barY);

	int fileInfoWidth = 0;
	TTF_SizeText(uiFont_, fileInfo.c_str(), &fileInfoWidth, nullptr);

	drawText(fileInfo, modeBg, w - fileInfoWidth - 5, barY);*/
}