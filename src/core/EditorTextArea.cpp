#include "EditorTextArea.hpp"

EditorTextArea::EditorTextArea(Config &config,ITextBuffer &textBuffer,Sdl &sdl)
    : config_(config), textBuffer_(textBuffer), sdl_(sdl) {}

void EditorTextArea::RenderText() {

    SDL_SetRenderDrawColor(sdl_.renderer_, 0, 0, 0, 255); // Black background
    SDL_RenderClear(sdl_.renderer_);

    static constexpr SDL_Color white = {255, 255, 255};
    for (size_t i{}; i < textBuffer_.size(); i++) {
        const size_t rowX = i * config_.font_.font_size;
        sdl_.RenderLine(textBuffer_.row(i), white, rowX);
    }

    SDL_RenderPresent(sdl_.renderer_);
}
