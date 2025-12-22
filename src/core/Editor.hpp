#pragma once
#include "buffer/Cursor.hpp"
#include "buffer/ITextBuffer.hpp"
#include "commands/Commands.hpp"
#include "utils/Config.hpp"

//Get event and then process it; action -> actual change
class Editor {
public:
    Editor() = delete;
    Editor(ITextBuffer& textBuffer, Cursor& cursor, const Config& config, Commands& commands);
    void HandleKeyboardInput();
    ~Editor() = default;

    ITextBuffer& textBuffer_;
    Cursor& cursor_;
    const Config& config_;
    Commands& commands_;
};
