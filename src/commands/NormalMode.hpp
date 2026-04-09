#pragma once

#include "NormalModeSubModes.hpp"

#include <string_view>
#include <unordered_map>

class EditorState;
class EditorInputAndOutput;
class File;
class FilesManager;
class TextBufferView;
class Cursor;
class Matrix;
class PanesManager;
class WindowSettings;

using namespace std::string_view_literals;

#define FUNC_TYPES Matrix &text, Cursor &cursor, EditorState &state
constexpr char lineChar = static_cast<char>(128);

enum class SpecialCases: char;

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

	std::unordered_map<char, Func> actions;
	std::unordered_map<char, Func2> operations;
	std::unordered_map<char, Func> motions;

	std::unordered_map<char, Func3> targetMotions;
	std::unordered_map<char, Func3> targetCommands;

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
};

enum class ParsingStages : char {
	Start,
	WaitingForMotion,
	WaitingForMotionTarget,
	WaitingForCommandTarget,
	Finish
};

class NormalModeCommand {
public:
	NormalModeCommand() = delete;
	NormalModeCommand(char operation, char motion, char targetMotion, char targetCommand, char targetChar, ParsingStages);
	~NormalModeCommand() noexcept = default;

	char operation;
	char motion;
	char targetMotion;
	char targetCommand;
	char targetChar;

	ParsingStages stage;
};

class NormalModeParser {
  public:
	explicit NormalModeParser(const NormalModeTable&);
	explicit NormalModeParser(const NormalModeTable&, char operation, char motion, char targetMotion, char targetCommand,
							  char targetChar, ParsingStages stage);
	~NormalModeParser() noexcept = default;

	void parseCommand(char);
	[[nodiscard]] bool executeCommand() const;
	void clear();
	NormalModeCommand getCommand() const;

  private:
	bool parseAction(char inputChar) const;
	bool parseOperation(char inputChar) const;
	bool parseMotion(char inputChar) const;
	bool parseTargetMotion(char inputChar) const;
	bool parseTargetCommand(char inputChar) const;

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
	bool shift{};
	NormalModeTable table;
	NormalModeParser parser;
	NormalModeExecutor executor;
};
