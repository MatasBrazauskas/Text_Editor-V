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

	const SDL_Color normalModeColor_ = {137, 180, 250};
	const SDL_Color insertModeColor_ = {195, 232, 141};
	const SDL_Color commandModeColor_ = {254, 198, 118};

	const SDL_Color windowModeColor_ = {203, 166, 247};
	const SDL_Color fileModeColor_ = {245, 194, 231};

	const Config& config_;
	const Settings& settings_;

	void RenderTabs(const TabLayout&) const;
	void RenderPanes(const std::vector<PanesLayout>&) const;
	void RenderRegularFile(const PanesLayout&) const;
	void RenderFileExplorer(const PanesLayout&) const;
	void RenderCursor(const CursorLayout&) const;
	void RenderCommandLine(const CommandLineLayout&) const;
};