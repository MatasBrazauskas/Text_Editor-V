#pragma once
#include <cstddef>
#include "utils/Config.hpp"
#include "graphics/Sdl.hpp"

class Cursor {
public:
    Cursor() = delete;
    Cursor(const Config& config);

    bool state;
    const Config& config;
    std::size_t x;
    std::size_t y;

    void RenderCursor(Sdl& sdl_) const;
};