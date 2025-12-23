#pragma once
#include "buffer/Cursor.hpp"
#include "buffer/ITextBuffer.hpp"
#include "commands/Insert.hpp"
#include "commands/Normal.hpp"
#include <memory>

//Get event and then process it; action -> actual change
class Editor {
public:
    Editor();
    Editor(ITextBuffer& textBuffer, Cursor& cursor);
    ~Editor() = default;

    void HandleKeyboardInput();
    void switchToInsertMode();
    void switchToNormalMode();

    ITextBuffer& textBuffer_;
    Cursor& cursor_;

    bool running_;
    std::string input_;

    std::unique_ptr<NormalMode> normalMode_;
    std::unique_ptr<InsertMode> insertMode_;
    IMode* mode_;
};
