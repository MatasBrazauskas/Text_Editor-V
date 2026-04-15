#include "CommandMode.hpp"

#include "core/EditorCore.hpp"

#include <iostream>
#include <sstream>

CommandStructure CommandMode::parseCommand(const std::string& t_input) const {
	if (t_input.empty()) {
		abort();
	}

	std::stringstream ss{t_input};
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
	commands_ = {{"q", &CommandMode::closeProgramme},
				 {"w", &CommandMode::writeToFile},
				 {"e", &CommandMode::openFile},
				 {"bn", &CommandMode::switchToNextBuffer},
				 {"bp", &CommandMode::switchToPrevBuffer}};

	specialKeybinds_ = {
		{static_cast<char>(SpecialKeys::LeftArrow), &CommandMode::moveCursorLeft},
		{static_cast<char>(SpecialKeys::RightArrow), &CommandMode::moveCursorRight},
		{static_cast<char>(SpecialKeys::Backspace), &CommandMode::deleteChar},
		{static_cast<char>(SpecialKeys::Enter), &CommandMode::executeCommand},
	};
}

void CommandMode::HandleKeyboardInput(EditorState& t_state, EditorInputAndOutput& t_io, FilesManager& t_filesManager,
									  PanesManager& t_panesManager, const char t_inputChar) {
	if (t_io.input_.empty())
		return;

	t_io.input_.pop_back();

	if (const auto it = specialKeybinds_.find(t_inputChar); it != specialKeybinds_.end()) {
		(this->*it->second)(t_state, t_io, t_filesManager, t_panesManager);
	} else {
		t_io.commandLineMessage_.insert(t_io.cursorIndexX, 1, t_inputChar);
		t_io.cursorIndexX++;

		if (t_io.commandLineState_ != CommandLineState::None) {
			t_io.cleanInputs();
			t_io.commandLineState_ = CommandLineState::None;
		}
	}
}

void CommandMode::writeToFile(EditorState&, EditorInputAndOutput&, FilesManager& t_filesManager, PanesManager&,
							  const CommandStructure& com) const {
	std::cout << "Writing to file...\n";

	if (com.args_.empty()) {
		t_filesManager.saveCurrentFile();
	} else {
		for (const auto& filename : com.args_) {
			const auto result = t_filesManager.getFileId(filename);

			if (result.has_value()) {
				const auto fileId = *result;
				t_filesManager.saveFile(fileId);
			}
		}
	}
}

void CommandMode::openFile(EditorState&, EditorInputAndOutput& t_io, FilesManager& files, PanesManager&,
						   const CommandStructure& com) const {
	std::cout << "Opening file...\n";

	if (com.args_.empty()) {
		t_io.setError(NotEnoughArguments);
		return;
	}

	for (const auto& fileNames : com.args_) {
		const auto path = std::filesystem::path(fileNames);

		files.addRegularFile(std::move(path));
	}
}

void CommandMode::closeProgramme(EditorState& state, EditorInputAndOutput& t_io, FilesManager&, PanesManager&,
								 const CommandStructure& com) const {
	if (not com.args_.empty()) {
		t_io.setError(TooMuchArguments);
		return;
	}
	state.running_ = false;
}

void CommandMode::switchToNextBuffer(EditorState&, EditorInputAndOutput& t_io, FilesManager& t_fileManager, PanesManager& t_panesManager,
									 const CommandStructure& t_com) const {

	if (not t_com.args_.empty()) {
		t_io.setError(TooMuchArguments);
		return;
	}

	const auto nextFileId = t_fileManager.switchToNextFile();
	auto& currPane = t_panesManager.getCurrPane();
	currPane.switchFileId(nextFileId);
}

void CommandMode::switchToPrevBuffer(EditorState&, EditorInputAndOutput& t_io, FilesManager& t_fileManager, PanesManager& t_panesManager,
									 const CommandStructure& t_com) const {
	if (not t_com.args_.empty()) {
		t_io.setError(TooMuchArguments);
		return;
	}
	const auto prevFileId = t_fileManager.switchToPrevFile();
	auto& currPane = t_panesManager.getCurrPane();
	currPane.switchFileId(prevFileId);
}

void CommandMode::moveCursorRight(EditorState&, EditorInputAndOutput& t_io, FilesManager&, PanesManager&) const {
	t_io.cursorIndexX = std::min(t_io.cursorIndexX + 1, static_cast<int>(t_io.commandLineMessage_.length()));
}

void CommandMode::moveCursorLeft(EditorState&, EditorInputAndOutput& t_io, FilesManager&, PanesManager&) const {
	t_io.cursorIndexX = std::max(t_io.cursorIndexX - 1, 0);
}

void CommandMode::deleteChar(EditorState& t_state, EditorInputAndOutput& t_io, FilesManager&, PanesManager&) const {
	if (t_io.commandLineMessage_.length() > 0) {
		t_io.cursorIndexX = std::max(t_io.cursorIndexX - 1, 0);
		t_io.commandLineMessage_.erase(t_io.cursorIndexX);
	} else {
		t_io.cleanInputs();
		t_state.currentMode_ = Modes::Normal;
	}
}

void CommandMode::executeCommand(EditorState& t_state, EditorInputAndOutput& t_io, FilesManager& t_filesManager,
								 PanesManager& t_panesManager) const {
	const auto& com = parseCommand(t_io.commandLineMessage_);

	if (const auto it = commands_.find(com.command_); it != commands_.end()) {
		const auto& func = it->second;
		(this->*func)(t_state, t_io, t_filesManager, t_panesManager, com);

		t_io.cleanInputs();
		t_state.currentMode_ = Modes::Normal;
	} else {
		t_io.setError(UnknownCommand + com.command_);
	}
}
