 #include "commands/Insert.hpp"

 std::string_view InsertMode::name() const noexcept {return "Insert"sv;}

 void InsertMode::HandleKeyboardInput(std::string& input, ITextBuffer& textBuffer, Cursor& cursor) {
     if (!input.empty()) {
        auto& row = textBuffer.rowRef(cursor.y_);
        row.insert(cursor.x_++, input);
        input.clear();
    }
 }
