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

enum class Modes : uint8_t;

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

	std::unordered_map<Modes, SDL_Color> colorMap_;

	const Config& config_;
	const Settings& settings_;

	void RenderTabs(const TabLayout&) const;
	void RenderPanes(const std::vector<PanesLayout>&) const;
	void RenderRegularFile(const PanesLayout&) const;
	void RenderFileExplorer(const PanesLayout&) const;
	void RenderCursor(const CursorLayout&) const;
	void RenderCommandLine(const CommandLineLayout&) const;
};