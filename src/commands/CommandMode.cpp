#include "CommandMode.hpp"

#include <iostream>

void CommandMode::HandleKeyboardInput(EditorState&, Document&) const {
	std::cout << "Command mode running...\n";
}
