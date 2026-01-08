#include "CommandMode.hpp"

#include <iostream>

#include "core/Editor.hpp"

void CommandMode::HandleKeyboardInput(EditorState& editorState, Document&) const {
	std::cout << "Command mode running...\n";

    if (editorState.input_.back() == static_cast<char>(SpecialKeys::Enter)) {
        editorState.running_ = false;
    }
}
