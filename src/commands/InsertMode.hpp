#pragma once

#include <unordered_map>


class EditorState;
class EditorInputAndOutput;
class File;
class Cursor;

class InsertMode final {
      public:
	InsertMode();
	~InsertMode() = default;

	void HandleKeyboardInput(EditorState&, EditorInputAndOutput& , File&, Cursor&) const;
	using Func = void (InsertMode::*)(File&, Cursor&) const;

private:
	void handleEnter(File&, Cursor&) const;
	void handleBackspace(File&, Cursor&) const;

	std::unordered_map<char, Func> commands_;
};