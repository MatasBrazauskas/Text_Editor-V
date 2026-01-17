#include "CommandMode.hpp"

#include <sstream>
#include <iostream>

#include "core/Editor.hpp"

CommandStructure CommandMode::parseCommand(std::string input) {
    if (input.empty()) {
        abort();
    }

    input.pop_back();
    input.erase(0,1 );

    std::stringstream ss{input};
    CommandStructure com;

    std::string arg;
    if (!(ss >> arg)) {
        abort();
    }

    com.command_ = std::string(arg);

    while (ss >> arg) {
        com.args_.emplace_back(arg);
    }

    return com;
}

CommandMode::CommandMode() {
    commands_ = {
        {"q", &CommandMode::closeProgramme}
    };
}

void CommandMode::HandleKeyboardInput(EditorState& state, Document& doc) {
	std::cout << "Command mode running...\n";

    if (state.input_.back() == static_cast<char>(SpecialKeys::Enter)) {
        const auto& com = parseCommand(state.input_);

        std::cout << "Command: " << com.command_ << '\n';

        if (const auto it = commands_.find(com.command_); it != commands_.end()) {
            const auto& func = it->second;
            (this->*func)(state, doc, com);
        }
    }
}

void CommandMode::closeProgramme(EditorState& state, Document&, const CommandStructure&) {
    state.running_ = false;
}
