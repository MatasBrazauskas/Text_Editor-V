#pragma once
#include "Sdl.hpp"
#include "buffer/Cursor.hpp"
#include "buffer/ITextBuffer.hpp"
#include "utils/Config.hpp"

class Renderer {
public:
    Renderer() = delete;
    Renderer(ITextBuffer& textBuffer, const Cursor& cursor, const Config& config, Sdl& sdl);
    ~Renderer() = default;

    void Render() const;

    int width;
    int height;

    ITextBuffer& textBuffer_;
    const Cursor& cursor_;
    const Config& config_;
    Sdl& sdl_;
};