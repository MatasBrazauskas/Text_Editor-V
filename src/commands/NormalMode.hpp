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
    NormalModeCommand(int count1, char operation, int count2, char motion, char textObject, char targetChar, bool ignoreCount, ParsingStages stage);
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
	NormalMode();
    ~NormalMode() noexcept = default;
    void HandleKeyboardInput(EditorState&, std::reference_wrapper<Document>);
//private:

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

	void motionDeleteChar(FUNC_TYPES) const;


	void findFirstCharRight(FUNC_TYPES) const;
	void findFirstCharLeft(FUNC_TYPES) const;



	void insertLineAbove(FUNC_TYPES);
	void insertLineBelow(FUNC_TYPES);

	void switchToInsertLeft(FUNC_TYPES);
	void switchToInsertRight(FUNC_TYPES);

    void updateView(TextBufferView&, const Cursor&);
    void parseCommand(std::string& input, char inputChar);
    void executeNormalModeCommand(std::unique_ptr<ITextBuffer>& text, Cursor& cursor, EditorState& state);

    using Func = void (NormalMode::*)(FUNC_TYPES) const;
    using Func2 = void (NormalMode::*)(FUNC_TYPES, const MotionRange&, const MotionRange&) const;

    std::unordered_map<char, Func2> operations;
    std::unordered_map<char, Func> motions;
    std::unordered_map<char, Func> textObjects;

    NormalModeCommand command;
};

using Func = void (NormalMode::*)(FUNC_TYPES) const;
using Func2 = void (NormalMode::*)(FUNC_TYPES, const MotionRange&, const MotionRange&) const;
