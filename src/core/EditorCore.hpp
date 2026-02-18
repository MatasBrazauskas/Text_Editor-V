#pragma once

#include "buffer/Files.hpp"
#include "commands/CommandMode.hpp"
#include "commands/InsertMode.hpp"
#include "commands/NormalMode.hpp"
#include "buffer/PanesAndLayers.hpp"

#include <SDL2/SDL_events.h>

enum class Modes : uint8_t { Normal, Insert, Command };

enum class SpecialKeys : char {
    Backspace = static_cast<char>(129),
    Enter = static_cast<char>(130),
    LeftArrow = static_cast<char>(131),
    UpArrow = static_cast<char>(132),
    RightArrow = static_cast<char>(133),
    DownArrow = static_cast<char>(134),
    Tab = static_cast<char>(135),
    Escape = static_cast<char>(136)
};

using FileId = uint_fast64_t;
using PaneId = uint_fast64_t;

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

class EditorCore final {
public:
	EditorCore() = delete;
	EditorCore(int argc, char** argv);
	~EditorCore() noexcept = default;

	void HandleKeyboardInput();

    const Files& getFiles() const;
    const PanesManager& getPanesManager() const;
    const EditorState& getEditorState() const;
    const EditorInputAndOutput& getEditorInputAndOutput() const;

    bool dirty;
private:

    FileHandler fileHandler_;

    Files files_;
    PanesManager panes_;

    EditorState editorState_;
    EditorInputAndOutput editorInputAndOutput_;

    NormalMode normalMode_;
	InsertMode insertMode_;
	CommandMode commandMode_;

	std::string EncodeInput(const SDL_Event&);
};