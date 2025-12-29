#include "CommandMode.hpp"
#include <iostream>

void CommandMode::HandleKeyboardInput(EditorState&, Document&) {
	std::cout << "Command mode running...\n";
}
