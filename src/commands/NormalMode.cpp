#include "commands/NormalMode.hpp"

#include "buffer/Files.hpp"
#include "core/EditorCore.hpp"

#include <SDL_clipboard.h>
#include <algorithm>
#include <iostream>
#include <ranges>

NormalModeTable::NormalModeTable() {
	actions = {
		{'O', &NormalModeTable::actionInsertLineAbove},		{'o', &NormalModeTable::actionInsertLineBelow},
		{'i', &NormalModeTable::actionSwitchToInsertLeft},	{'a', &NormalModeTable::actionSwitchToInsertRight},
		{':', &NormalModeTable::actionSwitchToCommandMode}, {'x', &NormalModeTable::actionDeleteChar},
	};

	operations = {{'d', &NormalModeTable::operationDeleteChar}, {'y', &NormalModeTable::operationCopyText}};

	motions = {{'h', &NormalModeTable::motionMoveCursorLeft},
			   {'j', &NormalModeTable::motionMoveCursorDown},
			   {'k', &NormalModeTable::motionMoveCursorUp},
			   {'l', &NormalModeTable::motionMoveCursorRight},
			   {'G', &NormalModeTable::motionMoveCursorBottomFile},
			   {'$', &NormalModeTable::motionMoveRightMost},
			   {'0', &NormalModeTable::motionMoveLeftMost},
			   {'^', &NormalModeTable::motionMoveLeftMostChar},
			   {'w', &NormalModeTable::motionStartOfNextWord},
			   {'b', &NormalModeTable::motionStartOfPrevWord},
			   {'W', &NormalModeTable::motionStartOfNextWORD},
			   {'B', &NormalModeTable::motionStartOfPrevWORD},
			   {'e', &NormalModeTable::motionEndOfWord},
			   {'E', &NormalModeTable::motionEndOfWORD}};

	targetMotions = {{'f', &NormalModeTable::findFirstCharLeft}, {'F', &NormalModeTable::findFirstCharRight}};

	targetCommands = {{'r', &NormalModeTable::replaceChar}};
}

NormalModeCommand::NormalModeCommand(char operation, char motion, char targetMotion, char targetCommand, char targetChar,
									 ParsingStages stage)
	: operation(operation), motion(motion), targetMotion(targetMotion), targetCommand(targetCommand), targetChar(targetChar), stage(stage) {
}

NormalModeParser::NormalModeParser(const NormalModeTable& table_t)
	: NormalModeParser(table_t, ' ', ' ', ' ', ' ', ' ', ParsingStages::Start) {}

NormalModeParser::NormalModeParser(const NormalModeTable& table_t, char operation, char motion, char targetMotion, char targetCommand,
								   char targetChar, ParsingStages stage)
	: table{table_t}, command{operation, motion, targetMotion, targetCommand, targetChar, stage} {}

bool NormalModeParser::parseAction(const char inputChar) const {
	return table.actions.contains(inputChar);
}

bool NormalModeParser::parseOperation(const char inputChar) const {
	return table.operations.contains(inputChar);
}

bool NormalModeParser::parseMotion(const char inputChar) const {
	return table.motions.contains(inputChar);
}

bool NormalModeParser::parseTargetMotion(const char inputChar) const {
	return table.targetMotions.contains(inputChar);
}

bool NormalModeParser::parseTargetCommand(const char inputChar) const {
	return table.targetCommands.contains(inputChar);
}

void NormalModeParser::parseCommand(char inputChar) {
	const auto addAction = [&] {
		command.operation = inputChar;
		command.stage = ParsingStages::Finish;
	};

	const auto addOperation = [&] {
		command.operation = inputChar;
		command.stage = ParsingStages::WaitingForMotion;
	};

	const auto addMotion = [&] {
		command.motion = inputChar;
		command.stage = ParsingStages::Finish;
	};

	const auto addLineMotion = [&] {
		command.motion = lineChar;
		command.stage = ParsingStages::Finish;
	};

	const auto addTargetMotion = [&] {
		command.targetMotion = inputChar;
		command.stage = ParsingStages::WaitingForMotionTarget;
	};

	const auto addTargetCommand = [&] {
		command.targetCommand = inputChar;
		command.stage = ParsingStages::WaitingForCommandTarget;
	};

	const auto addTargetChar = [&] {
		command.targetChar = inputChar;
		command.stage = ParsingStages::Finish;
	};

	const bool action = parseAction(inputChar);
	const bool operation = parseOperation(inputChar);
	const bool motion = parseMotion(inputChar);
	const bool targetMotion = parseTargetMotion(inputChar);
	const bool targetCommand = parseTargetCommand(inputChar);
	const bool repeatedOperationAsLineMotion =
		command.stage == ParsingStages::WaitingForMotion && command.operation == inputChar && (inputChar == 'd' || inputChar == 'y');

	if (command.stage == ParsingStages::Start) {
		if (action) {
			addAction();
		} else if (operation) {
			addOperation();
		} else if (motion) {
			addMotion();
		} else if (targetMotion) {
			addTargetMotion();
		} else if (targetCommand) {
			addTargetCommand();
		}
	} else if (command.stage == ParsingStages::WaitingForMotion) {
		if (repeatedOperationAsLineMotion) {
			addLineMotion();
		} else if (motion) {
			addMotion();
		} else if (targetMotion) {
			addTargetMotion();
		}
	} else if (command.stage == ParsingStages::WaitingForMotionTarget || command.stage == ParsingStages::WaitingForCommandTarget) {
		addTargetChar();
	}
}

bool NormalModeParser::executeCommand() const {
	return command.stage == ParsingStages::Finish;
}

void NormalModeParser::clear() {
	command.operation = ' ';
	command.motion = ' ';
	command.targetMotion = ' ';
	command.targetCommand = ' ';
	command.targetChar = ' ';
	command.stage = ParsingStages::Start;
}

NormalModeCommand NormalModeParser::getCommand() const {
	return command;
}

NormalModeExecutor::NormalModeExecutor(const NormalModeTable& table) : table{table} {}

void NormalModeExecutor::executeNormalModeCommand(Matrix& text, Cursor& t_cursor, EditorState& state, const NormalModeCommand command) {
	const auto action = table.actions.find(command.operation);
	const auto operation = table.operations.find(command.operation);
	const auto motion = table.motions.find(command.motion);
	const auto targetMotion = table.targetMotions.find(command.targetMotion);
	const auto targetCommand = table.targetCommands.find(command.targetCommand);

	auto startRange = MotionRange{.x = t_cursor.getX(), .y = t_cursor.getY()};
	auto endRange = startRange;

	if (action != table.actions.end()) {
		(table.*action->second)(text, t_cursor, state);
		endRange = MotionRange{.x = t_cursor.getX(), .y = t_cursor.getY()};
	}

	if (command.motion == lineChar) {
		table.motionLine(text, t_cursor, state, startRange, endRange);
	} else if (motion != table.motions.end()) {
		(table.*motion->second)(text, t_cursor, state);
		endRange = MotionRange{.x = t_cursor.getX(), .y = t_cursor.getY()};
	} else if (targetMotion != table.targetMotions.end()) {
		(table.*targetMotion->second)(text, t_cursor, state, command.targetChar);
		endRange = MotionRange{.x = t_cursor.getX(), .y = t_cursor.getY()};
	} else if (targetCommand != table.targetCommands.end()) {
		(table.*targetCommand->second)(text, t_cursor, state, command.targetChar);
		endRange = MotionRange{.x = t_cursor.getX(), .y = t_cursor.getY()};
	}

	const auto trueStart = MotionRange{.x = std::min(startRange.x, endRange.x), .y = std::min(startRange.y, endRange.y)};
	const auto trueEnd = MotionRange{.x = std::max(startRange.x, endRange.x), .y = std::max(startRange.y, endRange.y)};

	if (operation != table.operations.end()) {
		(table.*operation->second)(text, t_cursor, state, trueStart, trueEnd);

		if (command.operation == 'd') {
			t_cursor.setX(trueStart.x);
			t_cursor.setY(trueStart.y);
		}
	}
}

NormalMode::NormalMode() : parser{table}, executor{table} {}

void NormalMode::HandleKeyboardInput(File& file_t, Cursor& t_cursor, EditorState& state, EditorInputAndOutput& inOut) {
	auto& [text, stack, path, id] = file_t;

	if (not shift && inOut.input_.back() == static_cast<char>(SpecialKeys::Shift)) {
		shift = true;
		return;
	}

	if (shift) {
		switch (inOut.input_.back()) {
		case 'f':
			state.currentMode_ = Modes::FileMode;
			break;
		case 'w':
			state.currentMode_ = Modes::WindowMode;
			break;
		}
		shift = false;
		inOut.input_.clear();
		inOut.commandLineMessage_.clear();
		return;
	}

	parser.parseCommand(inOut.input_.back());

	const auto command = parser.getCommand();

	std::cout << "Parse mode: " << static_cast<int>(command.stage) << ", operation: " << command.operation << ", motion: " << command.motion
			  << ", target motion: " << command.targetMotion << ", target command: " << command.targetCommand
			  << ", target char: " << command.targetChar << '\n';

	if (parser.executeCommand()) {
		executor.executeNormalModeCommand(text, t_cursor, state, command);
		inOut.input_.clear();
		inOut.commandLineMessage_.clear();
		parser.clear();
	}
}

void NormalModeTable::operationDeleteChar(FUNC_TYPES, const MotionRange& start, const MotionRange& end) const {
	if (start.y == end.y) {
		text.deleteRange(start.y, start.x, end.x - start.x);
	} else {
		const auto endLineSuffix = std::string{text.getLine(end.y).substr(end.x)};

		const int startLineLen = text.getLineLength(start.y);
		text.deleteRange(start.y, start.x, startLineLen - start.x);

		for (int i = end.y; i > start.y; --i) {
			text.deleteLine(i);
		}

		text.insertRange(start.y, start.x, endLineSuffix);
	}
}

void NormalModeTable::operationCopyText(FUNC_TYPES, const MotionRange& start, const MotionRange& end) const {

	std::string clipboardBuffer;

	if (start.y == end.y) {
		const auto subView = text.getLineSubstr(start.y, start.x, end.x - start.x);
		clipboardBuffer = std::string{subView};
	} else {
		clipboardBuffer += std::string(text.getLineSubstr(start.y, start.x)) + "\n";

		for (int y = start.y + 1; y < end.y; ++y) {
			clipboardBuffer += std::string(text.getLineSubstr(y, 0)) + "\n";
		}

		clipboardBuffer += std::string(text.getLineSubstr(end.y, 0, end.x));
	}

	if (SDL_SetClipboardText(clipboardBuffer.c_str()) != 0) {
		std::cerr << "SDL Clipboard Error: " << SDL_GetError() << std::endl;
	}
}

void NormalModeTable::motionStartOfNextWord(FUNC_TYPES) const {
	const auto currLine = text.getLine(cursor.getY());
	const size_t startX = cursor.getX();

	if (startX >= currLine.length())
		return;

	const std::string alphabet = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_";
	const std::string space = " \t\r\n";

	char currentCh = currLine[startX];
	size_t nextPos = startX;

	if (isspace(currentCh)) {
		nextPos = currLine.find_first_not_of(space, startX);
	} else {
		std::string currentGroup = alphabet.contains(currentCh) ? alphabet : text.separators_;

		nextPos = currLine.find_first_not_of(currentGroup, startX);

		if (nextPos != std::string_view::npos && isspace(currLine[nextPos])) {
			nextPos = currLine.find_first_not_of(space, nextPos);
		}
	}

	if (nextPos == std::string_view::npos) {
		if (cursor.getY() + 1 < text.getLinesCount()) {
			cursor.incrementY();

			const auto nextLine = text.getLine(cursor.getY());
			size_t firstChar = nextLine.find_first_not_of(space);

			cursor.setX(firstChar == std::string_view::npos ? 0 : firstChar);
		} else {
			const int index = std::max(0, static_cast<int>(currLine.length()) - 1);
			cursor.setX(index);
		}
	} else {
		cursor.setX(nextPos);
	}
}

void NormalModeTable::motionStartOfNextWORD(FUNC_TYPES) const {
	const auto spaceSeparator = " "s;
	const auto currLine = text.getLine(cursor.getY());

	const std::size_t indexPunctuation = currLine.find_first_of(spaceSeparator, cursor.getX());

	if (indexPunctuation != std::string_view::npos) {
		const std::size_t index = currLine.find_first_not_of(spaceSeparator, indexPunctuation);

		if (index != std::string_view::npos) {
			cursor.setX(index);
			return;
		}
	}

	if (cursor.getY() + 1 >= text.getLinesCount()) { // second line check
		const int x = std::max(0, static_cast<int>(currLine.length()) - 1);
		cursor.setX(x);
		return;
	}

	cursor.incrementY();
	const auto nextLine = text.getLine(cursor.getY());
	const std::size_t index = nextLine.find_first_not_of(spaceSeparator, 0);

	if (index != std::string_view::npos) {
		cursor.setX(index);
	} else {
		const int x = std::max(0, static_cast<int>(nextLine.length()) - 1);
		cursor.setX(x);
	}
}

void NormalModeTable::motionStartOfPrevWord(FUNC_TYPES) const {
	const std::string alphabet = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_";
	const std::string space = " \t\r\n";

	int y = cursor.getY();
	int x = cursor.getX();

	if (x > 0)
		x--;
	else if (y > 0) {
		y--;
		x = text.getLine(y).empty() ? 0 : text.getLine(y).length() - 1;
	} else
		return;

	auto currLine = text.getLine(y);

	while (y >= 0 && (currLine.empty() || isspace(currLine[x]))) {
		if (currLine.empty()) {
			cursor.setX(0);
			cursor.setY(y);
			return;
		}

		size_t lastVisible = currLine.find_last_not_of(space, x);
		if (lastVisible != std::string_view::npos) {
			x = lastVisible;
			break;
		}
		if (y == 0) {
			cursor.setX(0);
			cursor.setY(0);
			return;
		}

		y--;
		currLine = text.getLine(y);
		x = currLine.empty() ? 0 : currLine.length() - 1;
	}

	char c = currLine[x];
	std::string currentGroup = alphabet.contains(c) ? alphabet : text.separators_;

	while (x > 0 && currentGroup.contains(currLine[x - 1])) {
		x--;
	}

	cursor.setY(y);
	cursor.setX(x);
}

void NormalModeTable::motionEndOfWord(FUNC_TYPES) const {
	int y = cursor.getY();
	int x = cursor.getX();
	auto currLine = text.getLine(y);
	const std::string alphabet = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_";
	const std::string space = " \t\r\n";

	if (x + 1 < currLine.length()) {
		x++;
	} else if (y + 1 < text.getLinesCount()) {
		y++;
		x = 0;
		currLine = text.getLine(y);
	} else
		return;

	while (y < text.getLinesCount() && (currLine.empty() || isspace(currLine[x]))) {
		size_t firstVisible = currLine.find_first_not_of(space, x);
		if (firstVisible != std::string_view::npos) {
			x = static_cast<int>(firstVisible);
			break;
		}
		if (++y >= text.getLinesCount())
			return;
		currLine = text.getLine(y);
		x = 0;
	}

	char c = currLine[x];
	std::string currentGroup = alphabet.contains(c) ? alphabet : text.separators_;

	size_t endOfGroup = currLine.find_first_not_of(currentGroup, x);

	if (endOfGroup == std::string_view::npos) {
		cursor.setX(static_cast<int>(currLine.length()) - 1);
		cursor.setY(y);
	} else {
		cursor.setX(static_cast<int>(endOfGroup) - 1);
		cursor.setY(y);
	}
}

void NormalModeTable::motionEndOfWORD(FUNC_TYPES) const {
	int y = cursor.getY();
	int x = cursor.getX();
	auto currLine = text.getLine(y);
	const std::string space = " \t\r\n";

	if (x + 1 < currLine.length())
		x++;
	else if (y + 1 < text.getLinesCount()) {
		y++;
		x = 0;
		currLine = text.getLine(y);
	} else
		return;

	while (y < text.getLinesCount() && (currLine.empty() || isspace(currLine[x]))) {
		size_t firstVisible = currLine.find_first_not_of(space, x);
		if (firstVisible != std::string_view::npos) {
			x = static_cast<int>(firstVisible);
			break;
		}
		if (++y >= text.getLinesCount())
			return;
		currLine = text.getLine(y);
		x = 0;
	}

	size_t nextSpace = currLine.find_first_of(space, x);

	if (nextSpace == std::string_view::npos) {
		cursor.setX(static_cast<int>(currLine.length()) - 1);
		cursor.setY(y);
	} else {
		cursor.setX(static_cast<int>(nextSpace) - 1);
		cursor.setY(y);
	}
}

void NormalModeTable::motionStartOfPrevWORD(FUNC_TYPES) const {
	const auto spaceSeparator = " "s;
	int y = cursor.getY();
	int x = cursor.getX();

	auto currLine = text.getLine(y);

	if (x == 0 || isspace(currLine[x]) || (!isspace(currLine[x]) && isspace(currLine[x - 1]))) {
		if (x == 0) {
			if (y == 0)
				return;
			y--;
			currLine = text.getLine(y);
			x = currLine.empty() ? 0 : currLine.length() - 1;
		} else {
			x--;
		}
	}

	while (y >= 0) {
		currLine = text.getLine(y);
		if (currLine.empty()) {
			cursor.setY(y);
			cursor.setX(0);
			return;
		}

		size_t lastNonSpace = currLine.find_last_not_of(spaceSeparator, x);
		if (lastNonSpace != std::string_view::npos) {
			x = static_cast<int>(lastNonSpace);
			break;
		}

		if (y == 0) {
			cursor.setX(0);
			cursor.setY(0);
			return;
		}

		y--;
		currLine = text.getLine(y);
		x = currLine.empty() ? 0 : currLine.length() - 1;
	}

	size_t startOfWord = currLine.find_last_of(spaceSeparator, x);
	cursor.setY(y);
	cursor.setX(startOfWord == std::string_view::npos ? 0 : startOfWord + 1);
}

void NormalModeTable::motionLine(FUNC_TYPES, MotionRange& start, MotionRange& end) const {
	start.x = 0;
	end.x = text.getLineLength(cursor.getY());
}

void NormalModeTable::motionMoveCursorLeft(FUNC_TYPES) const {
	if (cursor.getX() > 0) {
		cursor.decrementX();
	}
}

void NormalModeTable::motionMoveCursorRight(FUNC_TYPES) const {
	if (text.getLineLength(cursor.getY()) > cursor.getX()) {
		cursor.incrementX();
	}
}

void NormalModeTable::motionMoveCursorUp(FUNC_TYPES) const {
	if (cursor.getY() > 0) {
		const auto currRowLength = text.getLineLength(cursor.getY());
		cursor.decrementY();
		const auto nextRowLength = text.getLineLength(cursor.getY());

		if (nextRowLength == 0) {
			cursor.setX(0);
		} else if (currRowLength - 1 == cursor.getX() || nextRowLength - 1 < cursor.getX()) {
			cursor.setX(nextRowLength - 1);
		}
	}
}

void NormalModeTable::motionMoveCursorDown(FUNC_TYPES) const {
	if (text.getLinesCount() - 1 > cursor.getY()) {

		const size_t currRowLength = text.getLineLength(cursor.getY());

		cursor.incrementY();
		const std::size_t nextRowLength = text.getLineLength(cursor.getY());

		if (nextRowLength == 0) {
			cursor.setX(0);
		} else if (currRowLength - 1 == cursor.getX() || nextRowLength - 1 <= cursor.getX()) {
			cursor.setX(nextRowLength - 1);
		}
	}
}

void NormalModeTable::motionMoveCursorBottomFile(FUNC_TYPES) const {
	cursor.setY(text.getLinesCount() - 1);
	cursor.setX(std::min(text.getLineLength(cursor.getY()) - 1, cursor.getX()));
}

void NormalModeTable::motionMoveRightMost(FUNC_TYPES) const {
	if (text.getLineLength(cursor.getY()) > 0) {
		cursor.setX(text.getLineLength(cursor.getY()) - 1);
	}
}

void NormalModeTable::motionMoveLeftMostChar(FUNC_TYPES) const {
	const auto line = text.getLine(cursor.getY());

	const size_t index = line.find_first_not_of(" \t");

	if (index != std::string::npos) {
		cursor.setX(index);
	}
}

void NormalModeTable::motionMoveLeftMost(FUNC_TYPES) const {
	cursor.setX(0);
}

void NormalModeTable::actionDeleteChar(FUNC_TYPES) const {
	if (text.getLineLength(cursor.getY()) == 0) {
		return;
	}

	text.deleteCharacter(cursor.getY(), cursor.getX());

	if (text.getLineLength(cursor.getY()) == 0) {
		cursor.setX(0);
		return;
	}

	const std::size_t newIndex = std::min(cursor.getX(), text.getLineLength(cursor.getY()) - 1);
	cursor.setX(newIndex);
}

void NormalModeTable::actionInsertLineAbove(Matrix& text, Cursor& cursor, EditorState& state) const {

	text.insertLine(cursor.getY(), "");
	motionMoveCursorUp(text, cursor, state);

	cursor.setX(0);
	state.currentMode_ = Modes::Insert;
}

void NormalModeTable::actionInsertLineBelow(FUNC_TYPES) const {
	motionMoveCursorDown(text, cursor, state);
	text.insertLine(cursor.getY(), "");

	cursor.setX(0);
	state.currentMode_ = Modes::Insert;
}

void NormalModeTable::findFirstCharRight(FUNC_TYPES, const char newChar) const {
	const auto reversedView = text.getLineSubstr(cursor.getY(), 0, cursor.getX()) | std::views::reverse;
	const auto it = std::ranges::find(reversedView, newChar);

	if (it != reversedView.end()) {
		const auto offset = cursor.getX() - std::distance(reversedView.begin(), it) - 1;
		cursor.setX(offset);
	}
}

void NormalModeTable::findFirstCharLeft(FUNC_TYPES, const char newChar) const {
	const auto subView = text.getLineSubstr(cursor.getY(), cursor.getX() + 1);
	const auto it = std::ranges::find(subView, newChar);

	if (it != subView.end()) {
		cursor.setX(cursor.getX() + 1 + std::distance(subView.begin(), it));
	}
}

void NormalModeTable::replaceChar(FUNC_TYPES, const char newChar) const {
	if (text.getLineLength(cursor.getY())) {
		text.deleteCharacter(cursor.getY(), cursor.getX());
		text.insertCharacter(cursor.getY(), cursor.getX(), newChar);
	}
}

void NormalModeTable::actionSwitchToInsertLeft(FUNC_TYPES) const {
	state.currentMode_ = Modes::Insert;
}

void NormalModeTable::actionSwitchToInsertRight(FUNC_TYPES) const {
	motionMoveCursorRight(text, cursor, state);
	state.currentMode_ = Modes::Insert;
}

void NormalModeTable::actionSwitchToCommandMode(FUNC_TYPES) const {
	state.currentMode_ = Modes::Command;
}
