#pragma once

#include "utils/ConfigAndSettings.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <vector>

class ConfigAndSettings;
class TabLayout;
class PanesLayout;
class CursorLayout;
class CommandLineLayout;
class LayoutManager;

class Renderer final {
  public:
	Renderer() = delete;
	explicit Renderer(const Config&, const Settings&);
	~Renderer() noexcept;

	void Render(const LayoutManager&) const;

  private:
	SDL_Window* window_;
	SDL_Renderer* renderer_;
	TTF_Font* codeFont_;
	TTF_Font* uiFont_;

	SDL_Color normalModeColor_;
	SDL_Color insertModeColor_;
	SDL_Color commandModeColor_;

	const Config& config_;
	const Settings& settings_;

	void RenderTabs(const TabLayout&) const;
	void RenderPanes(const std::vector<PanesLayout>&) const;
	void RenderCursor(const CursorLayout&) const;
	void RenderCommandLine(const CommandLineLayout&) const;
};