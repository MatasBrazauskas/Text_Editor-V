#include "CommandMode.hpp"

#include <iostream>

#include "core/Editor.hpp"

void CommandMode::HandleKeyboardInput(EditorState&, Document&) const {
	std::cout << "Command mode running...\n";
}
