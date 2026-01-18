#pragma once

#include <memory>
#include <SDL_events.h>

#include "buffer/Files.hpp"
#include "commands/NormalMode.hpp"
#include "commands/InsertMode.hpp"
#include "commands/CommandMode.hpp"

enum class Modes: uint8_t {Normal, Insert, Command};

enum class SpecialKeys: uint8_t {Backspace = 129, Enter = 130};

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
	Editor(Files&, FileHandler&, EditorState&);
	~Editor() = default;

	void HandleKeyboardInput();
private:
	Files& files_;
    FileHandler& fileHandler_;
	EditorState& editorState_;

	NormalMode normalMode_;
	InsertMode insertMode_;
	CommandMode commandMode_;

    std::string EncodeInput(const SDL_Event&);
};