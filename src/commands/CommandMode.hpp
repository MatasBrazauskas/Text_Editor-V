#pragma once
#include "utils/FileHandler.hpp"

#include <string>
#include <unordered_map>
#include <vector>

class EditorState;
class Document;
class FilesAndCursor;

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
	void HandleKeyboardInput(EditorState&, FileHandler&, FilesAndCursor&);

	using Func = void (CommandMode::*)(EditorState&, FileHandler&, FilesAndCursor&, const CommandStructure&);

	void writeToFile(EditorState&, FileHandler&, FilesAndCursor&, const CommandStructure&);
	void openFile(EditorState&, FileHandler&, FilesAndCursor&, const CommandStructure&);
	void closeProgramme(EditorState&, FileHandler&, FilesAndCursor&, const CommandStructure&);

      private:
	CommandStructure parseCommand(std::string input);
	std::unordered_map<std::string, Func> commands_;
};