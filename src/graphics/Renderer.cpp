#include "Renderer.hpp"

Renderer::Renderer(const ITextBuffer& textBuffer, const Cursor& cursor, const Config& config, Sdl& sdl)
    : textBuffer_(textBuffer), cursor_(cursor), config_(config), sdl_(sdl) {}

void Renderer::RenderText() const {
    const auto [r,g,b,a] = config_.colors_.background_color;
    const auto textColor = config_.colors_.foreground_color;

    SDL_SetRenderDrawColor(sdl_.renderer_, r, g, b, a); // Black background
    SDL_RenderClear(sdl_.renderer_);

    for (auto i{0zu}; i < textBuffer_.size(); i++) {
        const size_t rowX = i * config_.font_.font_size;
        sdl_.RenderLine(textBuffer_.row(i), textColor, rowX);
    }

    SDL_RenderPresent(sdl_.renderer_);
}
