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

enum CommandState {
	WaitCount1 = 1,
	WaitCount2 = 2,
	WaitOperation = 4,
	WaitMotion = 8,
};

class Command {
      public:
	Command();
	~Command() = default;
	int state;
	int count;
	std::string operation;
	std::string motion;
	bool commandReady;

	std::string to_string(const Command& c) const {
		return std::format("({}, {}, {})", c.count, c.operation, c.motion);
	}
};

#define FUNC_TYPES std::unique_ptr<ITextBuffer>&text, Cursor &cursor, EditorState &state

class NormalMode final {
      public:
	NormalMode();
	void HandleKeyboardInput(EditorState&, std::reference_wrapper<Document>);
	using Func = void (NormalMode::*)(FUNC_TYPES);

      private:
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
	void deleteChar(FUNC_TYPES);
	void deleteWord(FUNC_TYPES);
	void deleteAllWord(FUNC_TYPES);

	void findFirstCharRight(FUNC_TYPES);
	void findFirstCharLeft(FUNC_TYPES);

	void insertLineAbove(FUNC_TYPES);
	void insertLineBelow(FUNC_TYPES);

	void switchToInsertLeft(FUNC_TYPES);
	void switchToInsertRight(FUNC_TYPES);

	void updateView(TextBufferView&, const Cursor&);

	std::unordered_map<std::string, Func> fixedCommands_;
	std::unordered_map<std::string, Func> paramCommands_;
	Func paramFunc_;
	size_t paramCount_;

	bool parseCount(int& count, std::string_view input);
	bool parseOperation(Command&, std::string_view input);
	bool parseMotion(Command&, std::string_view input);

	std::unordered_map<std::string_view, Func> operations_;
	std::unordered_map<std::string_view, Func> motions_;
	Command command_;
};