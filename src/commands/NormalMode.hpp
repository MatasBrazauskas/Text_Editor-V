#pragma once

#include <memory>
#include <string_view>
#include <unordered_map>

class EditorState;
class Document;
class ITextBuffer;
class TextBufferView;
class Cursor;

using namespace std::string_view_literals;

#define FUNC_TYPES std::unique_ptr<ITextBuffer>&text, Cursor &cursor, EditorState &state

struct MotionRange {
	int x, y;
};

enum class ParsingStages : char {
	Count1OperationMotionTextObject,
	Count2MotionTextObject,
	WaitingForTargetChar,
	Finish,
};

class NormalModeCommand {
      public:
	NormalModeCommand();
	NormalModeCommand(int count1, char operation, int count2, char motion, char textObject, char targetChar,
			  bool ignoreCount, ParsingStages stage);
	~NormalModeCommand() noexcept = default;

	int count1;
	char operation;

	int count2;
	char motion;
	char textObject;

	char targetChar;
	bool ignoreCount;

	ParsingStages stage;
};

class NormalMode {
      public:
	NormalModeCommand command;
	NormalMode();
	~NormalMode() noexcept = default;
	void HandleKeyboardInput(EditorState&, std::reference_wrapper<Document>);
	void parseCommand(std::string& input, char inputChar);
	void executeNormalModeCommand(std::unique_ptr<ITextBuffer>& text, Cursor& cursor, EditorState& state);

      private:
	void operationDeleteChar(FUNC_TYPES, const MotionRange&, const MotionRange&) const;
	void operationCopyText(FUNC_TYPES, const MotionRange&, const MotionRange&) const;

	void motionMoveCursorLeft(FUNC_TYPES) const;
	void motionMoveCursorDown(FUNC_TYPES) const;
	void motionMoveCursorUp(FUNC_TYPES) const;
	void motionMoveCursorRight(FUNC_TYPES) const;

	void motionMoveCursorBottomFile(FUNC_TYPES) const;

	void motionMoveRightMost(FUNC_TYPES) const;
	void motionMoveLeftMost(FUNC_TYPES) const;

	void motionMoveLeftMostChar(FUNC_TYPES) const;

	void actionDeleteChar(FUNC_TYPES) const;

	void findFirstCharRight(FUNC_TYPES) const;
	void findFirstCharLeft(FUNC_TYPES) const;

	void actionInsertLineAbove(FUNC_TYPES) const;
	void actionInsertLineBelow(FUNC_TYPES) const;

	void actionSwitchToInsertLeft(FUNC_TYPES) const;
	void actionSwitchToInsertRight(FUNC_TYPES) const;

	void motionStartOfNextWord(FUNC_TYPES) const;
	void motionStartOfNextWORD(FUNC_TYPES) const;

	void motionStartOfPrevWord(FUNC_TYPES) const;
	void motionStartOfPrevWORD(FUNC_TYPES) const;

    void motionEndOfWord(FUNC_TYPES) const;
    void motionEndOfWORD(FUNC_TYPES) const;

	void updateView(TextBufferView&, const Cursor&) const;

	using Func = void (NormalMode::*)(FUNC_TYPES) const;
	using Func2 = void (NormalMode::*)(FUNC_TYPES, const MotionRange&, const MotionRange&) const;

	bool parseCount(int count, char inputChar) const;
	bool parseAction(char inputChar) const;
	bool parseOperation(char inputChar) const;
	bool parseMotion(char inputChar) const;
	bool parseTextObject(char inputChar) const;

	std::unordered_map<char, Func2> operations;
	std::unordered_map<char, Func> actions;

	std::unordered_map<char, Func> motions;
	std::unordered_map<char, Func> textObjects;
};

using Func = void (NormalMode::*)(FUNC_TYPES) const;
using Func2 = void (NormalMode::*)(FUNC_TYPES, const MotionRange&, const MotionRange&) const;
