#pragma once

#include "buffer/Files.hpp"
#include "commands/CommandMode.hpp"
#include "commands/InsertMode.hpp"
#include "commands/NormalMode.hpp"
#include <buffer/Panes.hpp>

#include <SDL2/SDL_events.h>

enum class Modes : uint8_t { Normal, Insert, Command };

enum class SpecialKeys : char {
    Backspace = static_cast<char>(129),
    Enter = static_cast<char>(130)
};

using FileId = uint_fast64_t;

class EditorState final {
      public:
	EditorState() = delete;
    explicit EditorState(FileId);
	~EditorState() noexcept = default;

	Modes currentMode_;
    FileId activeFileId_;
	bool running_;
};

class EditorInputAndOutput final {
public:
    EditorInputAndOutput() = default;
    ~EditorInputAndOutput() noexcept = default;

    std::string input_;
    std::string commandLineMessage_;
};

class Editor final {
public:
	Editor() = delete;
	Editor(int argc, char** argv);
	~Editor() noexcept = default;

	void HandleKeyboardInput();

    FileHandler fileHandler_;

    Files files_;
    Panes panes_;

    EditorState editorState_;
    EditorInputAndOutput editorInputAndOutput_;

private:
	NormalMode normalMode_;
	InsertMode insertMode_;
	CommandMode commandMode_;

	std::string EncodeInput(const SDL_Event&);
};