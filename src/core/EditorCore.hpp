#pragma once

#include "buffer/Files.hpp"
#include "buffer/PanesAndLayers.hpp"
#include "commands/CommandMode.hpp"
#include "commands/InsertMode.hpp"
#include "commands/NormalMode.hpp"
#include "utils/ConfigAndSettings.hpp"

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
	Escape = static_cast<char>(136),
	Control = static_cast<char>(137)
};

static const std::unordered_map<SDL_Keycode, char> specialKeyMap = {
	{SDLK_BACKSPACE, static_cast<char>(SpecialKeys::Backspace)},
	{SDLK_RETURN, static_cast<char>(SpecialKeys::Enter)},
	{SDLK_LEFT, static_cast<char>(SpecialKeys::LeftArrow)},
	{SDLK_UP, static_cast<char>(SpecialKeys::UpArrow)},
	{SDLK_RIGHT, static_cast<char>(SpecialKeys::RightArrow)},
	{SDLK_DOWN, static_cast<char>(SpecialKeys::DownArrow)},
	{SDLK_TAB, static_cast<char>(SpecialKeys::Tab)},
	{SDLK_ESCAPE, static_cast<char>(SpecialKeys::Escape)},
	{SDLK_LCTRL, static_cast<char>(SpecialKeys::Control)}};

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
	EditorCore(int argc, char** argv, Settings&);
	~EditorCore() noexcept = default;

	void HandleKeyboardInput();

	FilesManager& getFilesManager();
	PanesManager& getPanesManager();
	const EditorState& getEditorState() const;
	const EditorInputAndOutput& getEditorInputAndOutput() const;

  private:
	Settings& settings_;
	FileHandler fileHandler_;

	FilesManager filesManager_;
	PanesManager panesManager_;

	EditorState editorState_;
	EditorInputAndOutput editorInputAndOutput_;

	NormalModeDistributor normalModeDistributor_;
	InsertMode insertMode_;
	CommandMode commandMode_;

	std::string EncodeInput(const SDL_Event&);
};