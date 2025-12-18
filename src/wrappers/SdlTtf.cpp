#include "SdlTtf.h"
#include <stdexcept>

Wrapper::SdlTtf::SdlTtf(const Utils::FontConfig& fontConfig) : font_(nullptr){
    if (TTF_Init() == -1) {
        throw std::runtime_error("Failed to initialize TTF.");
    }

    font_ = TTF_OpenFont(fontConfig.font_path.c_str(), static_cast<int>(fontConfig.font_size));

    if (!font_) {
        throw std::runtime_error("Failed to open font.");
    }

    TTF_SetFontHinting(font_, TTF_HINTING_LIGHT);
}

Wrapper::SdlTtf::~SdlTtf() {
    if (font_ != nullptr) {
        TTF_CloseFont(font_);
        font_ = nullptr;
    }
}
