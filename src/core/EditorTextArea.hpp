#pragma once

#include "graphics/Sdl.hpp"
#include "buffer/ITextBuffer.hpp"
#include "utils/Config.hpp"

class EditorTextArea {
public:
    EditorTextArea() = delete;
    EditorTextArea(Config& config, ITextBuffer& textBuffer, Sdl& window);
    ~EditorTextArea() = default;

    void RenderText();
//private:
    Config& config_;
    ITextBuffer& textBuffer_;
    Sdl& sdl_;
};