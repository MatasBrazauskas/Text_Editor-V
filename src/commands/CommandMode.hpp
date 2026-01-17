#pragma once
#include <string>
#include <vector>
#include <unordered_map>

class EditorState;
class Document;

class CommandStructure final {
public:
    CommandStructure() = default;
    ~CommandStructure() = default;
    std::string command_;
    std::vector<std::string> args_;
};


class CommandMode final {
public:
	CommandMode();
	void HandleKeyboardInput(EditorState&, Document&);

    using Func = void (CommandMode::*)(EditorState&, Document&, const CommandStructure&);

    void closeProgramme(EditorState&, Document&, const CommandStructure&);
private:
    CommandStructure parseCommand(std::string input);
    std::unordered_map<std::string, Func> commands_;
};