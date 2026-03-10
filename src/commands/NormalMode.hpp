#pragma once

#include <string_view>
#include <unordered_map>

class EditorState;
class EditorInputAndOutput;
class File;
class TextBufferView;
class Cursor;
class Matrix;
class PanesManager;

using namespace std::string_view_literals;

class WindowSubCommandTable final {
  public:
	WindowSubCommandTable();
	~WindowSubCommandTable() noexcept = default;

	using Func = void (WindowSubCommandTable::*)(PanesManager&) const;

	void verticalSplit(PanesManager&) const;
	void horizontalSplit(PanesManager&) const;

	void movePaneLeft(PanesManager&) const;
	void movePaneRight(PanesManager&) const;
	void movePaneDown(PanesManager&) const;
	void movePaneUp(PanesManager&) const;

	void closePane(PanesManager&) const;

  private:
	std::unordered_map<char, Func> functionMap_;
};

class WindowSubCommandExecutor final {
  public:
	WindowSubCommandExecutor() = delete;
	explicit WindowSubCommandExecutor(const WindowSubCommandTable&);
	~WindowSubCommandExecutor() noexcept = default;
};

#define FUNC_TYPES Matrix &text, Cursor &cursor, EditorState &state
constexpr char lineChar = static_cast<char>(128);

struct MotionRange {
	int x, y;
};

class NormalModeTable {
  public:
	NormalModeTable();
	~NormalModeTable() noexcept = default;
	using Func = void (NormalModeTable::*)(FUNC_TYPES) const;
	using Func2 = void (NormalModeTable::*)(FUNC_TYPES, const MotionRange&, const MotionRange&) const;
	using Func3 = void (NormalModeTable::*)(FUNC_TYPES, char newChar) const;

	std::unordered_map<char, Func2> operations;
	std::unordered_map<char, Func> actions;

	std::unordered_map<char, Func> motions;
	std::unordered_map<char, Func3> textObjects;

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

	void findFirstCharRight(FUNC_TYPES, char) const;
	void findFirstCharLeft(FUNC_TYPES, char) const;
	void replaceChar(FUNC_TYPES, char) const;

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

	void motionLine(FUNC_TYPES, MotionRange&, MotionRange&) const;

	void switchToWindowMode(FUNC_TYPES) const;
};

enum class ParsingStages : char {
	Count1OperationMotionTextObject,
	Count2MotionTextObject,
	WaitingForTargetChar,
	Finish,
};

struct NormalModeCommand {
	NormalModeCommand(int count1, char operation, int count2, char motion, char textObject, char targetChar,
					  bool ignoreCount, ParsingStages stage)
		: count1(count1), operation(operation), count2(count2), motion(motion), textObject(textObject),
		  targetChar(targetChar), ignoreCount(ignoreCount), stage(stage) {}
	int count1;
	int count2;

	char operation;

	char motion;
	char textObject;

	char targetChar;
	bool ignoreCount;

	ParsingStages stage;
};

class NormalModeParser {
  public:
	explicit NormalModeParser(const NormalModeTable&);
	explicit NormalModeParser(const NormalModeTable&, int count1, char operation, int count2, char motion,
							  char textObject, char targetChar, bool ignoreCount, ParsingStages stage);
	~NormalModeParser() noexcept = default;

	void parseCommand(std::string& input);
	[[nodiscard]] bool executeCommand() const;
	void clear();
	NormalModeCommand getCommand() const;

  private:
	bool parseCount1(char inputChar) const;
	bool parseCount2(char inputChar) const;
	bool parseAction(char inputChar) const;
	bool parseOperation(char inputChar) const;
	bool parseMotion(char inputChar) const;
	bool parseTextObject(char inputChar) const;

	const NormalModeTable& table;
	NormalModeCommand command;
};

class NormalModeExecutor {
  public:
	explicit NormalModeExecutor(const NormalModeTable& table);
	~NormalModeExecutor() noexcept = default;

	void executeNormalModeCommand(Matrix& text, Cursor& t_cursor, EditorState& state, const NormalModeCommand command);

  private:
	const NormalModeTable& table;
};

class NormalMode final {
  public:
	NormalMode();
	~NormalMode() noexcept = default;
	void HandleKeyboardInput(File&, Cursor&, EditorState&, EditorInputAndOutput&);

  private:
	NormalModeTable table;
	NormalModeParser parser;
	NormalModeExecutor executor;

	// void updateView(TextBufferView&, const Cursor&) const;
};