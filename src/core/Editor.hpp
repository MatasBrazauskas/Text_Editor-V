#pragma once

#include <memory>

#include "buffer/Files.hpp"
#include "commands/NormalMode.hpp"
#include "commands/InsertMode.hpp"
#include "commands/CommandMode.hpp"

enum class Modes: uint8_t {Normal, Insert, Command};

// for unified input clearance - SPACES, BACKSPACE detection, resize.
class HandleInput final {

};

class EditorState final {
public:
	EditorState();
	~EditorState() = default;


	std::string input_;
    std::string commandLineInput_;

	int activeTab_;
    Modes currentMode_;
    bool running_;
};

class Editor final {
public:
	Editor() = delete;

	Editor(Files&, EditorState&);

	~Editor() = default;

	void HandleKeyboardInput();

	Files& files_;
	EditorState& editorState_;

	NormalMode normalMode_;
	InsertMode insertMode_;
	CommandMode commandMode_;
};