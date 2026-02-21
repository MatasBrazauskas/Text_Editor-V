#include "Renderer.hpp"

#include "EditorCore.hpp"

#include <iostream>

#include "utils/Config.hpp"
#include "buffer/PanesAndLayers.hpp"

Renderer::Renderer(Config& t_config): config_{t_config} {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
        throw std::runtime_error(SDL_GetError());
    }

    window_ = SDL_CreateWindow(config_.editor_.title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        1000, 800, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE //| SDL_WINDOW_BORDERLESS
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

    ConstantsConfig constConfig{};
    auto& [codeCharWidth, codeCharHeight, uiCharWidth, uiCharHeight, tabHeight, paddingX] = constConfig;

    TTF_SetFontHinting(codeFont_, TTF_HINTING_MONO);
    TTF_SetFontKerning(codeFont_, 0);

    TTF_SizeText(codeFont_, "A", &codeCharWidth, &codeCharHeight);

    TTF_SetFontHinting(uiFont_, TTF_HINTING_MONO);
    TTF_SetFontKerning(uiFont_, 0);

    TTF_SizeText(uiFont_, "A", &uiCharWidth, &uiCharHeight);

    tabHeight = uiCharHeight + 5;
}

Renderer::~Renderer() noexcept {
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

void Renderer::Render(const LayoutManager & t_layoutManage) const {
    RenderTabs(t_layoutManage.tabLayout, t_layoutManage.windowHeight, t_layoutManage.windowWidth);
    RenderPanes(t_layoutManage.panesLayout);
    RenderCursor(t_layoutManage.cursorLayout);
    RenderCommandLine(t_layoutManage.commandLineLayout, t_layoutManage.windowHeight, t_layoutManage.windowWidth);
}


void Renderer::RenderTabs(const TabLayout& t_tabLayout, const int windowHeight, const int windowWidth) const {
    const auto& [activeTab, tabCapLines, tabs] = t_tabLayout;
	const auto constConfig = config_.constantConfig_;

	constexpr SDL_Color colBarBg = {30, 30, 30, 255};
	constexpr SDL_Color colTabInact = {45, 45, 45, 255};
	constexpr SDL_Color colTabAct = {30, 32, 40, 255};
	constexpr SDL_Color colAccent = {0, 122, 204, 255};
	constexpr SDL_Color colTextAct = {255, 255, 255, 255};
	constexpr SDL_Color colTextInact = {150, 150, 150, 255};

	int currentX{};
	int currentY{};

	const SDL_Rect barRect = {0, 0, windowWidth, tabCapLines * tabCapLines};
	SDL_SetRenderDrawColor(renderer_, colBarBg.r, colBarBg.g, colBarBg.b, colBarBg.a);
	SDL_RenderFillRect(renderer_, &barRect);

	for (auto i{0zu}; i< tabs.size(); i++) {
        const auto& filename = std::string{tabs.at(i)};

		int textW{}, textH{};
		TTF_SizeText(uiFont_, filename.c_str(), &textW, &textH);

		const int tabWidth = textW + (constConfig.paddingX * 2);
		const bool isActive = i == activeTab;

		SDL_Rect tabRect = {currentX, currentY, tabWidth, constConfig.uiCharHeight};

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
				   currentY + constConfig.uiCharHeight);

		SDL_Color textColor = isActive ? colTextAct : colTextInact;

		SDL_Surface* surface = TTF_RenderText_Blended(uiFont_, filename.c_str(), textColor);
		SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer_, surface);

		SDL_Rect textRect = {currentX + constConfig.paddingX, (constConfig.uiCharHeight - textH) / 2 + currentY, textW, textH};

		SDL_RenderCopy(renderer_, texture, nullptr, &textRect);

		SDL_FreeSurface(surface);
		SDL_DestroyTexture(texture);

		currentX += tabWidth;
	}
}

void Renderer::RenderPanes(const std::vector<PanesLayout>& panes) const {
    const auto& fg = config_.colors_.foreground_color;
	const auto constConfig = config_.constantConfig_;

    for (const auto& pane: panes) {
        const auto& [startX, startY, endX, endY, leftDataOffsetX, leftData, lines] = pane;

        for (auto i{0zu}; i < lines.size(); i++) {
            const auto& line = std::string{lines.at(i)};

            SDL_Surface* surface = TTF_RenderText_Blended(codeFont_, line.c_str(), fg);
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer_, surface);

            const int length = line.length() * constConfig.codeCharWidth;
            const int lineOffset = i * constConfig.codeCharHeight;

            const SDL_Rect src{0, 0, length, surface->h};
            const SDL_Rect dst{startX + leftDataOffsetX, startY + lineOffset * constConfig.codeCharHeight, length, surface->h};

            SDL_RenderCopy(renderer_, texture, &src, &dst);

            SDL_FreeSurface(surface);
            SDL_DestroyTexture(texture);
        }
    }
}

void Renderer::RenderCursor(const CursorLayout& t_cursorLayout) const {
    const auto [cr, cg, cb, ca] = config_.colors_.cursor_color;
    const auto& cursorFg = config_.colors_.selection_color;

    const auto& [active, cursorX, cursorY, letter, panesLayout] = t_cursorLayout;

    SDL_SetRenderDrawColor(renderer_, cr, cg, cb, ca);
	const auto& [codeCharWidth, codeCharHeight, uiCharWidth, uiCharHeight, tabHeight, paddingX] = config_.constantConfig_;


    const int cursorOffsetY = codeCharHeight;
    const int cursorOffsetX = codeCharWidth;

    if (active) {
        if (letter == '\0') {
            const auto rect = SDL_Rect{cursorOffsetX, cursorOffsetY, 1, codeCharHeight};

            SDL_RenderFillRect(renderer_, &rect);
        } else {
            const SDL_Rect cursorRect{cursorOffsetX, cursorOffsetY, codeCharWidth,
                          codeCharHeight};
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
	const auto& [codeCharWidth, codeCharHeight, uiCharWidth, uiCharHeight, tabHeight, paddingX] = config_.constantConfig_;
    const auto& [mode, currFileName, currCommand, cursorX, cursorY, charCount, lineCount, args] = t_commandLineLayout;
    std::string line{};
    SDL_Color bg;

    switch (mode) {
        case Modes::Normal:
            line = "Normal";
            bg = {137, 180, 250};
        case Modes::Insert:
            line = "Insert";
            bg ={195, 232, 141};
            break;
        case Modes::Command:
            line = "Command";
            bg = {254, 198, 118};
            break;
        default:
            abort();
    }
    const auto topLRect = SDL_Rect{0, windowHeight - uiCharHeight - uiCharHeight, windowWidth, uiCharHeight};

    SDL_SetRenderDrawColor(renderer_, bg.r, bg.g, bg.b, bg.a);
    SDL_RenderFillRect(renderer_, &topLRect);

    SDL_Surface* surface = TTF_RenderText_Blended(uiFont_, line.c_str(), config_.colors_.selection_color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer_, surface);

    const SDL_Rect dst{0, windowHeight - uiCharHeight - uiCharHeight, surface->w, surface->h};
    SDL_RenderCopy(renderer_, texture, nullptr, &dst);

    if (mode == Modes::Command) {
        const auto& temp = std::string{args};
        surface = TTF_RenderText_Blended(uiFont_, temp.c_str(), config_.colors_.cursor_color);

        // TODO fix this ugly mess
        const int lineLength = args.length();
        const auto& [r, g, b, a] = config_.colors_.cursor_color;
        SDL_SetRenderDrawColor(renderer_, r, g, b, a);
        const auto rect = SDL_Rect{lineLength * uiCharWidth, windowHeight - uiCharHeight, 1, codeCharHeight};

        SDL_RenderFillRect(renderer_, &rect);

        if (surface) {
            texture = SDL_CreateTextureFromSurface(renderer_, surface);

            const SDL_Rect dst2{0, windowHeight - uiCharHeight, surface->w, surface->h};

            SDL_RenderCopy(renderer_, texture, nullptr, &dst2);
        }
    }

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}