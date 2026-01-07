#pragma once

#include <functional>
#include <unordered_map>
#include <string_view>
#include <string>

class EditorState;
class Document;

using namespace std::string_view_literals;

using FixedFunc = std::function<void(EditorState&, Document&)>;
using ParamFunc = std::function<void(EditorState&, Document&)>;

class NormalMode final {
public:
	NormalMode();

	void HandleKeyboardInput(EditorState&, Document&);

private:
	void moveCursorLeft(EditorState&, Document&);
	void moveCursorDown(EditorState&, Document&);
	void moveCursorUp(EditorState&, Document&);
	void moveCursorRight(EditorState&, Document&);

	void moveCursorTopFile(EditorState&, Document&);
	void moveCursorBottomFile(EditorState&, Document&);

	void moveRightMost(EditorState&, Document&);
    void moveLeftMostChar(EditorState&, Document&);
	void moveLeftMost(EditorState&, Document&);

	void deleteLine(EditorState&, Document&);
	void deleteChar(EditorState&, Document&);
    void deleteWord(EditorState&, Document&);
    void deleteAllWord(EditorState&, Document&);

	void findFirstCharRight(EditorState&, Document&);
	void findFirstCharLeft(EditorState&, Document&);

	void insertLineAbove(EditorState&, Document&);
	void insertLineBelow(EditorState&, Document&);

	void switchToInsertLeft(EditorState&, Document&);
	void switchToInsertRight(EditorState&, Document&);

    void updateView(EditorState&, Document&);

	std::unordered_map<std::string, FixedFunc> fixedCommands_;
	std::unordered_map<std::string, ParamFunc> paramCommands_;

	ParamFunc paramFunc_;
	size_t paramCount_;
};