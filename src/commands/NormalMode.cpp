#include "commands/NormalMode.hpp"

#include "buffer/Files.hpp"
#include "core/Editor.hpp"

#include <SDL_clipboard.h>
#include <algorithm>
#include <iostream>
#include <ranges>

// TODO Motions work on ranges, not on cursor. Operation execute the range, not cursor position.

bool NormalModeParser::parseCount1(const char inputChar) const {
	if (!std::isdigit(inputChar))
		return false;

	if (inputChar == '0' && command.count1 == 0)
		return false;

	return true;
}

bool NormalModeParser::parseCount2(const char inputChar) const {
	if (!std::isdigit(inputChar))
		return false;

	if (inputChar == '0' && command.count2 == 0)
		return false;

	return true;
}

bool NormalModeParser::parseAction(const char inputChar) const {
	return table.actions.contains(inputChar);
}

bool NormalModeParser::parseOperation(const char inputChar) const {
	if (inputChar == 'r') {
		return true;
	}
	return table.operations.contains(inputChar);
}

bool NormalModeParser::parseMotion(const char inputChar) const {
	if (command.operation == inputChar && (inputChar == 'y' || inputChar == 'd')) {
		return true;
	}
	return table.motions.contains(inputChar);
}

bool NormalModeParser::parseTextObject(const char inputChar) const {
	return table.textObjects.contains(inputChar);
}

NormalModeParser::NormalModeParser(const NormalModeTable& table_t)
    : NormalModeParser(table_t, 0, ' ', 0, ' ', ' ', ' ', false, ParsingStages::Count1OperationMotionTextObject) {}

NormalModeParser::NormalModeParser(const NormalModeTable& table_t, const int count1, const char operation,
				   const int count2, const char motion, const char textObject, const char targetChar,
				   const bool ignoreCount, const ParsingStages stage)
    : table{table_t}, command{count1, operation, count2, motion, textObject, targetChar, ignoreCount, stage} {}

void NormalModeParser::parseCommand(std::string& input) {
	const char inputChar = input.back();

	auto addCount = [&, inputChar](int& cnt) {
		cnt *= 10;
		cnt += inputChar - '0';
	};

	const auto addAction = [&] {
		command.operation = inputChar;
		command.stage = ParsingStages::Finish;
	};

	const auto addOperation = [&] {
		command.operation = inputChar;
		command.stage = ParsingStages::Count2MotionTextObject;

		if (inputChar == 'r') {
			command.ignoreCount = true;
			command.textObject = inputChar;
			command.operation = ' ';
			command.stage = ParsingStages::WaitingForTargetChar;
		}
	};

	const auto addMotion = [&] {
		if (command.operation == inputChar && (inputChar == 'y' || inputChar == 'd')) {
			command.motion = lineChar;
		} else {
			command.motion = inputChar;
		}
		command.stage = ParsingStages::Finish;
	};

	const auto addTextObject = [&] {
		command.textObject = inputChar;
		command.stage = ParsingStages::WaitingForTargetChar;
	};

	const auto addTargetChar = [&] {
		command.targetChar = inputChar;
		command.stage = ParsingStages::Finish;
	};

	const auto clearInputs = [&] { input.clear(); };

	const bool count1 = parseCount1(inputChar);
	const bool action = parseAction(inputChar);
	const bool operation = parseOperation(inputChar);
	const bool count2 = parseCount2(inputChar);
	const bool motion = parseMotion(inputChar);
	const bool textObject = parseTextObject(inputChar);

	if (command.stage == ParsingStages::Count1OperationMotionTextObject) {
		if (count1) {
			addCount(command.count1);
		} else if (action) {
			addAction();
		} else if (operation) {
			addOperation();
		} else if (motion) {
			addMotion();
		} else if (textObject) {
			addTextObject();
		} else {
			clearInputs();
		}
	} else if (command.stage == ParsingStages::Count2MotionTextObject) {
		if (count2) {
			addCount(command.count2);
		} else if (motion) {
			addMotion();
		} else if (textObject) {
			addTextObject();
		} else {
			clearInputs();
		}
	} else if (command.stage == ParsingStages::WaitingForTargetChar) {
		addTargetChar();
	}
}

bool NormalModeParser::executeCommand() const {
	return command.stage == ParsingStages::Finish;
}

void NormalModeParser::clear() {
	command.count1 = 0;
	command.count2 = 0;
	command.operation = ' ';
	command.motion = ' ';
	command.textObject = ' ';
	command.targetChar = ' ';
	command.ignoreCount = false;
	command.stage = ParsingStages::Count1OperationMotionTextObject;
}

NormalModeCommand NormalModeParser::getCommand() const {
	return command;
}

void NormalMode::HandleKeyboardInput(EditorState& state, const std::reference_wrapper<Document> doc) {
	auto& [text, view, cursor, _] = doc.get();

	parser.parseCommand(state.input_);

	const auto command = parser.getCommand();

	std::cout << "Parse mode: " << static_cast<int>(command.stage) << ". Count1: " << command.count1
		  << ", operation: " << command.operation << ", count2: " << command.count2
		  << ", motion: " << command.motion << ", text object: " << command.textObject
		  << ", target char: " << command.targetChar << '\n';

	if (parser.executeCommand()) {
		executor.executeNormalModeCommand(text, cursor, state, command);
		parser.clear();
		state.input_.clear();
		updateView(view, cursor);
	}
}

NormalMode::NormalMode() : parser{table}, executor{table} {}

NormalModeExecutor::NormalModeExecutor(const NormalModeTable& table) : table{table} {}

void NormalModeExecutor::executeNormalModeCommand(std::unique_ptr<ITextBuffer>& text, Cursor& cursor, EditorState& state,
						  const NormalModeCommand command) {
	const auto action = table.actions.find(command.operation);
	const auto operation = table.operations.find(command.operation);
	const auto motion = table.motions.find(command.motion);
	const auto textObject = table.textObjects.find(command.textObject);

	const auto startRange = MotionRange{.x = cursor.getX(), .y = cursor.getY()};

	const std::size_t loopCount =
	    command.ignoreCount ? 1 : std::max(1, command.count1) * std::max(1, command.count2);

	for (auto i{0zu}; i < loopCount; ++i) {
		if (action != table.actions.end()) {
			(&table->*action->second)(text, cursor, state);
		}
		if (motion != table.motions.end()) {
			(&table->*motion->second)(text, cursor, state);
		} else if (textObject != table.textObjects.end()) {
			(&table->*textObject->second)(text, cursor, state, command.targetChar);
		}
	}

	const auto endRange = MotionRange{.x = cursor.getX(), .y = cursor.getY()};

	const auto trueStart =
	    MotionRange{.x = std::min(startRange.x, endRange.x), .y = std::min(startRange.y, endRange.y)};
	const auto trueEnd =
	    MotionRange{.x = std::max(startRange.x, endRange.x), .y = std::max(startRange.y, endRange.y)};

	if (operation != table.operations.end()) {
		(&table->*operation->second)(text, cursor, state, trueStart, trueEnd);

		if (command.operation == 'd') {
			cursor.setX(trueStart.x);
			cursor.setY(trueStart.y);
		}
	}
}

NormalModeTable::NormalModeTable() {

	operations = {{'d', &NormalModeTable::operationDeleteChar}, {'y', &NormalModeTable::operationCopyText}};

	actions = {
	    {'O', &NormalModeTable::actionInsertLineAbove},    {'o', &NormalModeTable::actionInsertLineBelow},
	    {'i', &NormalModeTable::actionSwitchToInsertLeft}, {'a', &NormalModeTable::actionSwitchToInsertRight},
	    {'x', &NormalModeTable::actionDeleteChar},
	};

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

	textObjects = {{'f', &NormalModeTable::findFirstCharLeft},
		       {'F', &NormalModeTable::findFirstCharRight},
		       {'r', &NormalModeTable::replaceChar}};
}

void NormalModeTable::operationDeleteChar(FUNC_TYPES, const MotionRange& start, const MotionRange& end) const {
	if (start.y == end.y) {
		text->deleteRange(start.y, start.x, end.x - start.x);
	} else {
		const auto endLineSuffix = std::string{text->rowsView(end.y).substr(end.x)};

		const int startLineLen = text->rowsLength(start.y);
		text->deleteRange(start.y, start.x, startLineLen - start.x);

		for (int i = end.y; i > start.y; --i) {
			text->deleteLine(i);
		}

		text->insertRange(start.y, start.x, endLineSuffix);
	}
}

void NormalModeTable::operationCopyText(FUNC_TYPES, const MotionRange& start, const MotionRange& end) const {

	std::string clipboardBuffer;

	if (start.y == end.y) {
		const auto subView = text->rowSubstr(start.y, start.x, end.x - start.x);
		clipboardBuffer = std::string{subView};
	} else {
		clipboardBuffer += std::string(text->rowSubstr(start.y, start.x)) + "\n";

		for (int y = start.y + 1; y < end.y; ++y) {
			clipboardBuffer += std::string(text->rowSubstr(y, 0)) + "\n";
		}

		clipboardBuffer += std::string(text->rowSubstr(end.y, 0, end.x));
	}

	if (SDL_SetClipboardText(clipboardBuffer.c_str()) != 0) {
		std::cerr << "SDL Clipboard Error: " << SDL_GetError() << std::endl;
	}
}

void NormalModeTable::motionStartOfNextWord(FUNC_TYPES) const {
	const auto currLine = text->rowsView(cursor.getY());
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
		std::string currentGroup = alphabet.contains(currentCh) ? alphabet : text->separators;

		nextPos = currLine.find_first_not_of(currentGroup, startX);

		if (nextPos != std::string_view::npos && isspace(currLine[nextPos])) {
			nextPos = currLine.find_first_not_of(space, nextPos);
		}
	}

	if (nextPos == std::string_view::npos) {
		if (cursor.getY() + 1 < text->linesCount()) {
			cursor.incrementY();

			const auto nextLine = text->rowsView(cursor.getY());
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
	const auto currLine = text->rowsView(cursor.getY());

	const std::size_t indexPunctuation = currLine.find_first_of(spaceSeparator, cursor.getX());

	if (indexPunctuation != std::string_view::npos) {
		const std::size_t index = currLine.find_first_not_of(spaceSeparator, indexPunctuation);

		if (index != std::string_view::npos) {
			cursor.setX(index);
			return;
		}
	}

	if (cursor.getY() + 1 >= text->linesCount()) { // second line check
		const int x = std::max(0, static_cast<int>(currLine.length()) - 1);
		cursor.setX(x);
		return;
	}

	cursor.incrementY();
	const auto nextLine = text->rowsView(cursor.getY());
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
		x = text->rowsView(y).empty() ? 0 : text->rowsView(y).length() - 1;
	} else
		return;

	auto currLine = text->rowsView(y);

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
		currLine = text->rowsView(y);
		x = currLine.empty() ? 0 : currLine.length() - 1;
	}

	char c = currLine[x];
	std::string currentGroup = alphabet.contains(c) ? alphabet : text->separators;

	while (x > 0 && currentGroup.contains(currLine[x - 1])) {
		x--;
	}

	cursor.setY(y);
	cursor.setX(x);
}

void NormalModeTable::motionEndOfWord(FUNC_TYPES) const {
	int y = cursor.getY();
	int x = cursor.getX();
	auto currLine = text->rowsView(y);
	const std::string alphabet = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_";
	const std::string space = " \t\r\n";

	if (x + 1 < currLine.length()) {
		x++;
	} else if (y + 1 < text->linesCount()) {
		y++;
		x = 0;
		currLine = text->rowsView(y);
	} else
		return;

	while (y < text->linesCount() && (currLine.empty() || isspace(currLine[x]))) {
		size_t firstVisible = currLine.find_first_not_of(space, x);
		if (firstVisible != std::string_view::npos) {
			x = static_cast<int>(firstVisible);
			break;
		}
		if (++y >= text->linesCount())
			return;
		currLine = text->rowsView(y);
		x = 0;
	}

	char c = currLine[x];
	std::string currentGroup = alphabet.contains(c) ? alphabet : text->separators;

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
	auto currLine = text->rowsView(y);
	const std::string space = " \t\r\n";

	if (x + 1 < currLine.length())
		x++;
	else if (y + 1 < text->linesCount()) {
		y++;
		x = 0;
		currLine = text->rowsView(y);
	} else
		return;

	while (y < text->linesCount() && (currLine.empty() || isspace(currLine[x]))) {
		size_t firstVisible = currLine.find_first_not_of(space, x);
		if (firstVisible != std::string_view::npos) {
			x = static_cast<int>(firstVisible);
			break;
		}
		if (++y >= text->linesCount())
			return;
		currLine = text->rowsView(y);
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

	auto currLine = text->rowsView(y);

	if (x == 0 || isspace(currLine[x]) || (!isspace(currLine[x]) && isspace(currLine[x - 1]))) {
		if (x == 0) {
			if (y == 0)
				return;
			y--;
			currLine = text->rowsView(y);
			x = currLine.empty() ? 0 : currLine.length() - 1;
		} else {
			x--;
		}
	}

	while (y >= 0) {
		currLine = text->rowsView(y);
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
		currLine = text->rowsView(y);
		x = currLine.empty() ? 0 : currLine.length() - 1;
	}

	size_t startOfWord = currLine.find_last_of(spaceSeparator, x);
	cursor.setY(y);
	cursor.setX(startOfWord == std::string_view::npos ? 0 : startOfWord + 1);
}

void NormalModeTable::motionLine(FUNC_TYPES, MotionRange& start, MotionRange& end) const {
	start.x = 0;
	end.x = text->rowsLength(cursor.getY()) - 1;
}

void NormalMode::updateView(TextBufferView& view, const Cursor& cursor) const {
	if (cursor.getX() < view.startX_) {
		view.startX_ = cursor.getX();
	} else if (cursor.getX() >= view.startX_ + view.visibleColumns_) {
		view.startX_ = cursor.getX() - view.visibleColumns_ + 1;
	}

	if (cursor.getY() < view.startY_) {
		view.startY_ = cursor.getY();
	} else if (cursor.getY() >= view.startY_ + view.visibleLines_) {
		view.startY_ = cursor.getY() - view.visibleLines_ + 1;
	}
}

void NormalModeTable::motionMoveCursorLeft(FUNC_TYPES) const {
	if (cursor.getX() > 0) {
		cursor.decrementX();
	}
}

void NormalModeTable::motionMoveCursorRight(FUNC_TYPES) const {
	if (text->rowsLength(cursor.getY()) - 1 > cursor.getX()) {
		cursor.incrementX();
	}
}

void NormalModeTable::motionMoveCursorUp(FUNC_TYPES) const {
	if (cursor.getY() > 0) {
		const auto currRowLength = text->rowsLength(cursor.getY());
		cursor.decrementY();
		const auto nextRowLength = text->rowsLength(cursor.getY());

		if (nextRowLength == 0) {
			cursor.setX(0);
		} else if (currRowLength - 1 == cursor.getX() || nextRowLength - 1 < cursor.getX()) {
			cursor.setX(nextRowLength - 1);
		}
	}
}

void NormalModeTable::motionMoveCursorDown(FUNC_TYPES) const {
	if (text->linesCount() - 1 > cursor.getY()) {

		const size_t currRowLength = text->rowsLength(cursor.getY());

		cursor.incrementY();
		const std::size_t nextRowLength = text->rowsLength(cursor.getY());

		if (nextRowLength == 0) {
			cursor.setX(0);
		} else if (currRowLength - 1 == cursor.getX() || nextRowLength - 1 <= cursor.getX()) {
			cursor.setX(nextRowLength - 1);
		}
	}
}

void NormalModeTable::motionMoveCursorBottomFile(FUNC_TYPES) const {
	cursor.setY(text->linesCount() - 1);
	cursor.setX(std::min(text->rowsLength(cursor.getY()) - 1, cursor.getX()));
}

void NormalModeTable::motionMoveRightMost(FUNC_TYPES) const {
	if (text->rowsLength(cursor.getY()) > 0) {
		cursor.setX(text->rowsLength(cursor.getY()) - 1);
	}
}

void NormalModeTable::motionMoveLeftMostChar(FUNC_TYPES) const {
	const auto line = text->rowsView(cursor.getY());

	const size_t index = line.find_first_not_of(" \t");

	if (index != std::string::npos) {
		cursor.setX(index);
	}
}

void NormalModeTable::motionMoveLeftMost(FUNC_TYPES) const {
	cursor.setX(0);
}

void NormalModeTable::actionDeleteChar(FUNC_TYPES) const {
	if (text->rowsLength(cursor.getY()) == 0) {
		return;
	}

	text->deleteCharacter(cursor.getY(), cursor.getX());

	if (text->rowsLength(cursor.getY()) == 0) {
		cursor.setX(0);
		return;
	}

	const std::size_t newIndex = std::min(cursor.getX(), text->rowsLength(cursor.getY()) - 1);
	cursor.setX(newIndex);
}

// TODO fix the insert lines tho with count
void NormalModeTable::actionInsertLineAbove(std::unique_ptr<ITextBuffer>& text, Cursor& cursor,
					    EditorState& state) const {

	text->insertLine(cursor.getY(), "");
	motionMoveCursorUp(text, cursor, state);

	cursor.setX(0);
	state.currentMode_ = Modes::Insert;
}

void NormalModeTable::actionInsertLineBelow(FUNC_TYPES) const {
	motionMoveCursorDown(text, cursor, state);
	text->insertLine(cursor.getY(), "");

	cursor.setX(0);
	state.currentMode_ = Modes::Insert;
}

void NormalModeTable::findFirstCharRight(FUNC_TYPES, const char newChar) const {
	const auto reversedView = text->rowSubstr(cursor.getY(), 0, cursor.getX()) | std::views::reverse;
	const auto it = std::ranges::find(reversedView, newChar);

	if (it != reversedView.end()) {
		const auto offset = cursor.getX() - std::distance(reversedView.begin(), it) - 1;
		cursor.setX(offset);
	}
}

void NormalModeTable::findFirstCharLeft(FUNC_TYPES, const char newChar) const {
	const auto subView = text->rowSubstr(cursor.getY(), cursor.getX() + 1);
	const auto it = std::ranges::find(subView, newChar);

	if (it != subView.end()) {
		cursor.setX(cursor.getX() + 1 + std::distance(subView.begin(), it));
	}
}

void NormalModeTable::replaceChar(FUNC_TYPES, const char newChar) const {
	if (text->rowsLength(cursor.getY())) {
		text->deleteCharacter(cursor.getY(), cursor.getX());
		text->insertCharacter(cursor.getY(), cursor.getX(), newChar);
	}
}

void NormalModeTable::actionSwitchToInsertLeft(FUNC_TYPES) const {
	state.currentMode_ = Modes::Insert;
}

void NormalModeTable::actionSwitchToInsertRight(FUNC_TYPES) const {
	motionMoveCursorRight(text, cursor, state);
	state.currentMode_ = Modes::Insert;
}