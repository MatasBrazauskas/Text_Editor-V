#pragma once

#include <SDL_ttf.h>
#include "utils/Config.hpp"

namespace Wrapper {
    class SdlTtf {
    public:
        SdlTtf(const Utils::FontConfig& fontConfig);
        SdlTtf() = delete;
        ~SdlTtf();
    private:
        TTF_Font* font_;
    };
}