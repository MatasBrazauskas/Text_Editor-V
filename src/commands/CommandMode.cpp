#include "CommandMode.hpp"

#include "buffer/Matrix.hpp"
#include "core/EditorCore.hpp"

#include <iostream>
#include <sstream>

CommandStructure CommandMode::parseCommand(std::string input) {
	if (input.empty()) {
		abort();
	}

	input.pop_back();
	input.erase(0, 1);

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
	    {"q", &CommandMode::closeProgramme}, {"w", &CommandMode::writeToFile}, {"e", &CommandMode::openFile}};
}

void CommandMode::HandleKeyboardInput(EditorState& state, FileHandler& fileHandler, Files& files) {
	if (state.input_.empty())
		return;

	if (state.input_.back() == static_cast<char>(SpecialKeys::Backspace)) {
		if (state.input_.length() > 2) {
			state.input_.erase(state.input_.end() - 2, state.input_.end());
		} else {
			state.input_.clear();
			state.currentMode_ = Modes::Normal;
		}

	} else if (state.input_.back() == static_cast<char>(SpecialKeys::Enter)) {
		const auto& com = parseCommand(state.input_);

		if (const auto it = commands_.find(com.command_); it != commands_.end()) {
			const auto& func = it->second;
			(this->*func)(state, fileHandler, files, com);

			state.input_.clear();
			state.currentMode_ = Modes::Normal;
		} else {
			state.input_ = "Unknown command: " + com.command_;
		}
	}
}

void CommandMode::writeToFile(EditorState& state, FileHandler& fileHandler, Files& files, const CommandStructure& com) {
	std::cout << "Writing to file...\n";
	if (com.args_.empty()) {
		fileHandler.writeToFile(files.getDocument(state.activeTab_).value());
	} else {
		for (const auto& fileNames : com.args_) {
		}
	}
}

void CommandMode::openFile(EditorState& state, FileHandler& fileHandler, Files& files, const CommandStructure& com) {
	std::cout << "Opening file...\n";

	if (com.args_.empty()) {
		throw new std::runtime_error("No arguments for open command");
	}

	for (const auto& fileNames : com.args_) {
		const auto path = std::filesystem::path(fileNames);

		auto it = std::make_unique<Matrix>();
		const auto& file = fileHandler.getContent(path);
		it->init(file);

		files.addDocument(std::move(it), std::move(path));
	}
}

void CommandMode::closeProgramme(EditorState& state, FileHandler&, Files&, const CommandStructure& com) {
	if (!com.args_.empty()) {
		state.input_ = "Trailing characters";
	} else {
		state.running_ = false;
	}
}
