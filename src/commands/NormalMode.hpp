#pragma once

#include <format>
#include <memory>
#include <string>
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
	TextObject,
	Finish,
};

class NormalModeCommand {
      public:
	NormalModeCommand();
	~NormalModeCommand() noexcept = default;

	int count1;
	std::string operation;
	int count2;
	std::string motion;
	std::string textObject;
	ParsingStages stage;
};

class NormalMode final {
      public:
	NormalMode();
	void HandleKeyboardInput(EditorState&, std::reference_wrapper<Document>);
	using Func = void (NormalMode::*)(FUNC_TYPES);
    using Func2 = void (NormalMode::*)(FUNC_TYPES, const MotionRange&, const MotionRange&);

      private:
	std::unordered_map<std::string, Func2> operations;
	std::unordered_map<std::string, Func> motions;
	std::unordered_map<std::string, Func> textObjects;

	bool parseOperation(NormalModeCommand&, char inputChar);
	bool parseMotion(NormalModeCommand&, char inputChar);
	bool parseTextObject(NormalModeCommand&, char inputChar);
    void executeNormalModeCommand(std::unique_ptr<ITextBuffer>&text, Cursor &cursor, EditorState &state);

	Func paramFunc_;
	size_t paramCount_;

	NormalModeCommand command;

	void moveCursorLeft(FUNC_TYPES);
	void moveCursorDown(FUNC_TYPES);
	void moveCursorUp(FUNC_TYPES);
	void moveCursorRight(FUNC_TYPES);

	void moveCursorTopFile(FUNC_TYPES);
	void moveCursorBottomFile(FUNC_TYPES);

	void moveRightMost(FUNC_TYPES);
	void moveLeftMostChar(FUNC_TYPES);
	void moveLeftMost(FUNC_TYPES);

	void deleteLine(FUNC_TYPES);
	void deleteChar(FUNC_TYPES, const MotionRange&, const MotionRange&);
	void deleteWord(FUNC_TYPES);
	void deleteAllWord(FUNC_TYPES);

	void findFirstCharRight(FUNC_TYPES);
	void findFirstCharLeft(FUNC_TYPES);

	void insertLineAbove(FUNC_TYPES);
	void insertLineBelow(FUNC_TYPES);

	void switchToInsertLeft(FUNC_TYPES);
	void switchToInsertRight(FUNC_TYPES);

	void currentLine(FUNC_TYPES);

	void updateView(TextBufferView&, const Cursor&);
};