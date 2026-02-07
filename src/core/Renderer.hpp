#pragma once

#include "core/Editor.hpp"
#include "utils/Config.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

inline int codeCharWidth{}, codeCharHeight{};
inline int uiCharWidth{}, uiCharHeight{};
inline int tabOffsetY{};

class RenderWindow final {
      public:
	RenderWindow(std::size_t index_t, int offsetX_t, int offsetY_t, int width_t, int height_t);

	void Render(const Document&, const Config&, SDL_Renderer&, TTF_Font&, Modes) const;

	int offsetX_, offsetY_;
	int width_, height_;

	std::size_t index;

      private:
	void RenderText(const Document&, const Config&, SDL_Renderer&, TTF_Font&) const;
	void RenderLine(const Config&, SDL_Renderer&, TTF_Font&, std::string_view, int) const;
	void RenderCursor(const Document&, const Config&, SDL_Renderer&, TTF_Font&, Modes) const;
};

class RenderScreen final {
      public:
	RenderScreen() = delete;

	RenderScreen(const EditorState&, Files&, const Config&);

	~RenderScreen() noexcept;

	void Render();

	int windowWidth_;
	int windowHeight_;

	SDL_Window* window_;
	SDL_Renderer* renderer_;
	TTF_Font* codeFont_;
	TTF_Font* uiFont_;

	const EditorState& editorState_;
	Files& files_;
	const Config& config_;

	std::vector<RenderWindow> windows_;

	void addWindow(const RenderWindow&);
	void removeWindow(const RenderWindow&);

      private:
	void RenderTabs();
	void RenderCommandLine() const;
};