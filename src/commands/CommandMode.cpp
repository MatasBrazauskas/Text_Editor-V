#include "CommandMode.hpp"

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

void CommandMode::HandleKeyboardInput(EditorState& state, EditorInputAndOutput& t_io, FileHandler& fileHandler, FilesManager& files) {
	if (t_io.input_.empty())
		return;

	if (t_io.input_.back() == static_cast<char>(SpecialKeys::Backspace)) {
		if (t_io.input_.length() > 2) {
			t_io.input_.erase(t_io.input_.end() - 2, t_io.input_.end());
		} else {
			t_io.input_.clear();
			state.currentMode_ = Modes::Normal;
		}

	} else if (t_io.input_.back() == static_cast<char>(SpecialKeys::Enter)) {
		const auto& com = parseCommand(t_io.input_);

		if (const auto it = commands_.find(com.command_); it != commands_.end()) {
			const auto& func = it->second;
			(this->*func)(state, t_io, fileHandler, files, com);

			t_io.input_.clear();
			state.currentMode_ = Modes::Normal;
		} else {
			t_io.input_ = "Unknown command: " + com.command_;
		}
	}
}

void CommandMode::writeToFile(EditorState& state, EditorInputAndOutput&, FileHandler& fileHandler, FilesManager& files, const CommandStructure& com) {
	std::cout << "Writing to file...\n";
	std::cout << "Implement this shit\n";
	/*if (com.args_.empty()) {
		fileHandler.writeToFile(files.getDocument(state.activeTab_).value());
	} else {
		for (const auto& fileNames : com.args_) {
		}
	}*/
}

void CommandMode::openFile(EditorState& state, EditorInputAndOutput&, FileHandler& fileHandler, FilesManager& files, const CommandStructure& com) {
	std::cout << "Opening file...\n";

	if (com.args_.empty()) {
		throw new std::runtime_error("No arguments for open command");
	}

	for (const auto& fileNames : com.args_) {
		const auto path = std::filesystem::path(fileNames);

		const auto& file = fileHandler.getContent(path);
		files.addRegularFile(Matrix(std::move(file)), std::move(path));
	}
}

void CommandMode::closeProgramme(EditorState& state, EditorInputAndOutput& t_io, FileHandler&, FilesManager&, const CommandStructure& com) {
	if (!com.args_.empty()) {
		t_io.commandLineMessage_ = "Trailing characters";
	} else {
		state.running_ = false;
	}
}
