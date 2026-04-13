#pragma once
#include "utils/FileHandler.hpp"

#include <string>
#include <unordered_map>
#include <vector>

class EditorState;
class EditorInputAndOutput;
class Document;
class FilesManager;
class PanesManager;

using namespace std::string_literals;

constexpr auto NotEnoughArguments = "Not enough arguments.";
constexpr auto UnknownCommand = "Unknown command:";
constexpr auto TooMuchArguments = "Too much arguments.";

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
	void HandleKeyboardInput(EditorState&, EditorInputAndOutput&, FilesManager&, PanesManager&);

	using Func = void (CommandMode::*)(EditorState&, EditorInputAndOutput&, FilesManager&, PanesManager&, const CommandStructure&) const;

	void writeToFile(EditorState&, EditorInputAndOutput&, FilesManager&, PanesManager&, const CommandStructure&) const;
	void openFile(EditorState&, EditorInputAndOutput&, FilesManager&, PanesManager&, const CommandStructure&) const;
	void closeProgramme(EditorState&, EditorInputAndOutput&, FilesManager&, PanesManager&, const CommandStructure&) const;

	void switchToNextBuffer(EditorState&, EditorInputAndOutput&, FilesManager&, PanesManager&, const CommandStructure&) const;
	void switchToPrevBuffer(EditorState&, EditorInputAndOutput&, FilesManager&, PanesManager&, const CommandStructure&) const;

	void moveCursorRight(EditorState&, EditorInputAndOutput&, FilesManager&, PanesManager&, const CommandStructure&) const;
	void moveCursorLeft(EditorState&, EditorInputAndOutput&, FilesManager&, PanesManager&, const CommandStructure&) const;

  private:
	CommandStructure parseCommand(std::string input) const;
	std::unordered_map<std::string, Func> commands_;
};