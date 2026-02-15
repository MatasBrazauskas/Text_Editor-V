#pragma once

#include <vector>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

class Config;
class TabLayout;
class PanesLayout;
class CursorLayout;
class CommandLineLayout;
class LayoutManager;

class Renderer final {
public:
    Renderer() = delete;
    explicit Renderer(const Config&);
    ~Renderer() noexcept;

    void Render(const LayoutManager&) const;
private:
    int codeCharWidth{}, codeCharHeight{};
    int uiCharWidth{}, uiCharHeight{};

    SDL_Window* window_;
    SDL_Renderer* renderer_;
    TTF_Font* codeFont_;
    TTF_Font* uiFont_;

    const Config& config_;

    void RenderTabs(const TabLayout&, int windowHeight, int windowWidth) const;
    void RenderPanes(const std::vector<PanesLayout>&) const;
    void RenderCursor(const CursorLayout&) const;
    void RenderCommandLine(const CommandLineLayout&, int windowHeight, int windowWidth) const;
};