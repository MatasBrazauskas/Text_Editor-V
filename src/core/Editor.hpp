#pragma once

#include <memory>

#include "buffer/Files.hpp"
#include "commands/NormalMode.hpp"
#include "commands/InsertMode.hpp"
#include "commands/CommandMode.hpp"

enum class Modes: uint8_t {Normal, Insert, Command};


class EditorState final {
public:
	EditorState();
	~EditorState() = default;

	Modes currentMode_;

	bool running_;
	std::string input_;

	size_t activeTab_;

	std::string commandLineInput_;
};

class Editor final {
public:
	Editor() = delete;

	Editor(Files&, EditorState&);

	~Editor() = default;

	void HandleKeyboardInput();

	Files& files_;
	EditorState& editorState_;

	std::unique_ptr<NormalMode> normalMode_;
	std::unique_ptr<InsertMode> insertMode_;
	std::unique_ptr<CommandMode> commandMode_;
};