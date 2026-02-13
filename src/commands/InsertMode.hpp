#pragma once

#include <unordered_map>


class EditorState;
class EditorInputAndOutput;
class File;

class InsertMode final {
      public:
	InsertMode();
	~InsertMode() = default;

	void HandleKeyboardInput(EditorState&, EditorInputAndOutput& , std::reference_wrapper<File>) const;
	using Func = void (InsertMode::*)(File&) const;

      private:
	void handleEnter(File&) const;
	void handleBackspace(File&) const;

	std::unordered_map<char, Func> commands_;
};