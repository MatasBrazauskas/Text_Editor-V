#include "SdlInput.hpp"

#include <filesystem>

SdlInput::SdlInput() : event_() {}

const std::string& SdlInput::getInput() {
    buffer_.clear();

    SDL_PollEvent(&event_);
        if (event_.type == SDL_TEXTINPUT) {
            buffer_.append(event_.text.text);
        }

    return buffer_;
}