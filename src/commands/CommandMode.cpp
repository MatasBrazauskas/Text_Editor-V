#include "CommandMode.hpp"

#include "core/EditorCore.hpp"

#include <iostream>
#include <sstream>

CommandStructure CommandMode::parseCommand(std::string input) const {
	if (input.empty()) {
		abort();
	}

	input.pop_back();

	std::stringstream ss{input};
	CommandStructure com;

	std::string arg;
	if (not(ss >> arg)) {
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
		{"q", &CommandMode::closeProgramme},	  {"w", &CommandMode::writeToFile},			{"e", &CommandMode::openFile},
		{"bn", &CommandMode::switchToNextBuffer}, {"bp", &CommandMode::switchToPrevBuffer},
	};
}

void CommandMode::HandleKeyboardInput(EditorState& state, EditorInputAndOutput& t_io, FileHandler& fileHandler, FilesManager& files,
									  PanesManager& t_panesManager) {
	if (t_io.input_.empty())
		return;

	if (t_io.input_.back() == static_cast<char>(SpecialKeys::Backspace)) {
		if (t_io.input_.length() > 2) {
			t_io.removeLastInputChar();
		} else {
			t_io.cleanInputs();
			state.currentMode_ = Modes::Normal;
		}

	} else if (t_io.input_.back() == static_cast<char>(SpecialKeys::Enter)) {
		const auto& com = parseCommand(t_io.input_);

		if (const auto it = commands_.find(com.command_); it != commands_.end()) {
			const auto& func = it->second;
			(this->*func)(state, t_io, fileHandler, files, t_panesManager, com);

			t_io.cleanInputs();
			state.currentMode_ = Modes::Normal;
		} else {
			t_io.commandLineMessage_ = UnknownCommand + com.command_;
			t_io.commandLineError_ = true;
		}
	} else {
		if (t_io.commandLineError_) {
			t_io.cleanInputs();
			t_io.commandLineError_ = false;
		}
	}
}

void CommandMode::writeToFile(EditorState&, EditorInputAndOutput&, FileHandler& t_fileHandler, FilesManager& t_filesManager, PanesManager&,
							  const CommandStructure& com) const {
	std::cout << "Writing to file...\n";

	if (com.args_.empty()) {
		const auto& currentFile = t_filesManager.getFile();
		t_fileHandler.writeToFile(currentFile);
	} else {
		for (const auto& file : t_filesManager.files_) {
			t_fileHandler.writeToFile(file);
		}
	}
}

void CommandMode::openFile(EditorState&, EditorInputAndOutput& t_io, FileHandler& fileHandler, FilesManager& files, PanesManager&,
						   const CommandStructure& com) const {
	std::cout << "Opening file...\n";

	if (com.args_.empty()) {
		t_io.commandLineMessage_ = NotEnoughArguments;
		t_io.commandLineError_ = true;
		return;
	}

	for (const auto& fileNames : com.args_) {
		const auto path = std::filesystem::path(fileNames);

		const auto& file = fileHandler.readFile(path);
		files.addRegularFile(Matrix(std::move(file)), std::move(path));
	}
}

void CommandMode::closeProgramme(EditorState& state, EditorInputAndOutput& t_io, FileHandler&, FilesManager&, PanesManager&,
								 const CommandStructure& com) const {
	if (not com.args_.empty()) {
		t_io.commandLineMessage_ = TooMuchArguments;
		t_io.commandLineError_ = true;
		return;
	}
	state.running_ = false;
}

void CommandMode::switchToNextBuffer(EditorState&, EditorInputAndOutput& t_io, FileHandler&, FilesManager& t_fileManager,
									 PanesManager& t_panesManager, const CommandStructure& t_com) const {

	if (not t_com.args_.empty()) {
		t_io.commandLineMessage_ = TooMuchArguments;
		t_io.commandLineError_ = true;
		return;
	}

	const auto nextFileId = t_fileManager.switchToNextFile();
	const auto& currPane = t_panesManager.getCurrPane();
	currPane->get().fileId_ = nextFileId;
}

void CommandMode::switchToPrevBuffer(EditorState&, EditorInputAndOutput& t_io, FileHandler&, FilesManager& t_fileManager,
									 PanesManager& t_panesManager, const CommandStructure& t_com) const {
	if (not t_com.args_.empty()) {
		t_io.commandLineMessage_ = TooMuchArguments;
		t_io.commandLineError_ = true;
		return;
	}
	const auto prevFileId = t_fileManager.switchToPrevFile();
	const auto& currPane = t_panesManager.getCurrPane();
	currPane->get().fileId_ = prevFileId;
}