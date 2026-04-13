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
		{std::string{1, static_cast<char>(SpecialKeys::LeftArrow)}, &CommandMode::moveCursorLeft},
		{std::string{1, static_cast<char>(SpecialKeys::RightArrow)}, &CommandMode::moveCursorRight}
	};
}

void CommandMode::HandleKeyboardInput(EditorState& state, EditorInputAndOutput& t_io, FilesManager& files, PanesManager& t_panesManager) {
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
			(this->*func)(state, t_io, files, t_panesManager, com);

			t_io.cleanInputs();
			state.currentMode_ = Modes::Normal;
		} else {
			t_io.setError(UnknownCommand + com.command_);
		}
	} else {
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

void CommandMode::moveCursorRight(EditorState&, EditorInputAndOutput& t_io, FilesManager&, PanesManager&, const CommandStructure&) const {
}

void CommandMode::moveCursorLeft(EditorState&, EditorInputAndOutput&, FilesManager&, PanesManager&, const CommandStructure&) const {

}
