#pragma once
#include <string>
#include <vector>
#include <unordered_map>

#include "utils/FileHandler.hpp"

class EditorState;
class Document;
class Files;

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
	void HandleKeyboardInput(EditorState&, FileHandler&, Files&);

    using Func = void (CommandMode::*)(EditorState&, FileHandler&, Files&, const CommandStructure&);

    void writeToFile(EditorState&, FileHandler&, Files&, const CommandStructure&);
    void openFile(EditorState&, FileHandler &, Files &, const CommandStructure &);
    void closeProgramme(EditorState&, FileHandler&, Files&, const CommandStructure&);
private:
    CommandStructure parseCommand(std::string input);
    std::unordered_map<std::string, Func> commands_;
};