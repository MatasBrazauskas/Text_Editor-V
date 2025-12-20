#pragma once

#include <SDL.h>
#include <string>

class SdlInput {
public:
    SdlInput();
    ~SdlInput() = default;
    const std::string& getInput();

    std::string buffer_;
    SDL_Event event_;
};