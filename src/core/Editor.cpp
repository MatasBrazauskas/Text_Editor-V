#include "Editor.hpp"

Editor::Editor(ITextBuffer& textBuffer, Cursor& cursor, const Config& config, Commands& commands)
    : textBuffer_(textBuffer), cursor_(cursor), config_(config), commands_(commands) {}

void Editor::HandleKeyboardInput() {

}
