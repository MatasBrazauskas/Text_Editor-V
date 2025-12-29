#pragma once

#include <functional>
#include <unordered_map>
#include <string_view>
#include <string>

#include "commands/IMode.hpp"

using namespace std::string_view_literals;

using FixedFunc = std::function<void(EditorState&, Document&)>;
using ParamFunc = std::function<void(EditorState&, Document&)>;

class NormalMode final : public IMode {
public:
	NormalMode();

	void HandleKeyboardInput(EditorState&, Document&) override;

	void moveCursorLeft(EditorState&, Document&);

	void moveCursorDown(EditorState&, Document&);

	void moveCursorUp(EditorState&, Document&);

	void moveCursorRight(EditorState&, Document&);

	void moveCursorTopFile(EditorState&, Document&);

	void moveCursorBottomFile(EditorState&, Document&);

	void moveRightMost(EditorState&, Document&);

	void moveLeftMost(EditorState&, Document&);

	void deleteLine(EditorState&, Document&);

	void findFirstCharRight(EditorState&, Document&);
	void findFirstCharLeft(EditorState&, Document&);

	std::unordered_map<std::string, FixedFunc> fixedCommands_;
	std::unordered_map<std::string, ParamFunc> paramCommands_;

	ParamFunc paramFunc;
};