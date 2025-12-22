#include "EditorTextArea.hpp"

EditorTextArea::EditorTextArea(Config &config,ITextBuffer &textBuffer,Sdl &sdl)
    : config_(config), textBuffer_(textBuffer), sdl_(sdl) {}

void EditorTextArea::RenderText() {
    const auto bgColor = config_.colors_.background_color;
    const auto textColor = config_.colors_.foreground_color;

    SDL_SetRenderDrawColor(sdl_.renderer_, bgColor.r, bgColor.g, bgColor.b, bgColor.a); // Black background
    SDL_RenderClear(sdl_.renderer_);

    for (size_t i{}; i < textBuffer_.size(); i++) {
        const size_t rowX = i * config_.font_.font_size;
        sdl_.RenderLine(textBuffer_.row(i), textColor, rowX);
    }

    SDL_RenderPresent(sdl_.renderer_);
}
