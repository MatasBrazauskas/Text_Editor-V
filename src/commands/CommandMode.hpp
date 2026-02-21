#pragma once
#include "utils/FileHandler.hpp"

#include <string>
#include <unordered_map>
#include <vector>

class EditorState;
class EditorInputAndOutput;
class Document;
class FilesManager;

class CommandStructure final {
      public:
	CommandStructure() = default;
	~CommandStructure() noexcept = default;
	std::string command_;
	std::vector<std::string> args_;
};

class CommandMode final {
      public:
	CommandMode();
	~CommandMode() noexcept = default;
	void HandleKeyboardInput(EditorState&, EditorInputAndOutput&, FileHandler&, FilesManager&);

	using Func = void (CommandMode::*)(EditorState&, EditorInputAndOutput&, FileHandler&, FilesManager&, const CommandStructure&);

	void writeToFile(EditorState&, EditorInputAndOutput&, FileHandler&, FilesManager&, const CommandStructure&);
	void openFile(EditorState&, EditorInputAndOutput&, FileHandler&, FilesManager&, const CommandStructure&);
	void closeProgramme(EditorState&, EditorInputAndOutput&, FileHandler&, FilesManager&, const CommandStructure&);

      private:
	CommandStructure parseCommand(std::string input);
	std::unordered_map<std::string, Func> commands_;
};