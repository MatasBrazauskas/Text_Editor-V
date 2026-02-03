#include "commands/NormalMode.hpp"

#include "buffer/Files.hpp"
#include "core/Editor.hpp"

#include <SDL_clipboard.h>
#include <algorithm>
#include <iostream>
#include <ranges>

bool NormalMode::parseCount(const int count, const char inputChar) const {
	if (!std::isdigit(inputChar))
		return false;

	if (inputChar == '0' && count == 0)
		return false;

	return true;
}

bool NormalMode::parseAction(const char inputChar) const {
	return actions.contains(inputChar);
}

bool NormalMode::parseOperation(const char inputChar) const {
	return operations.contains(inputChar);
}

bool NormalMode::parseMotion(const char inputChar) const {
	return motions.contains(inputChar);
}

bool NormalMode::parseTextObject(const char inputChar) const {
	return textObjects.contains(inputChar);
}

NormalModeCommand::NormalModeCommand()
    : count1{0}, operation{' '}, count2{0}, motion{' '}, textObject{' '}, targetChar{' '}, ignoreCount{},
      stage{ParsingStages::Count1OperationMotionTextObject} {}

NormalModeCommand::NormalModeCommand(int count1, char operation, int count2, char motion, char textObject,
				     char targetChar, bool ignoreCount, ParsingStages stage)
    : count1{count1}, operation{operation}, count2{count2}, motion{motion}, textObject{textObject},
      targetChar{targetChar}, ignoreCount{ignoreCount}, stage{stage} {}

void NormalMode::parseCommand(std::string& input, const char inputChar) {
	auto addCount = [inputChar](int& count) {
		count *= 10;
		count += inputChar - '0';
	};

	const auto addAction = [inputChar](NormalModeCommand& com) {
		com.operation = inputChar;
		com.stage = ParsingStages::Finish;
	};

	const auto addOperation = [inputChar](NormalModeCommand& com) {
		com.operation = inputChar;
		com.stage = ParsingStages::Count2MotionTextObject;
	};

	const auto addMotion = [inputChar](NormalModeCommand& com) {
		com.motion = inputChar;
		com.stage = ParsingStages::Finish;
	};

	const auto addTextObject = [inputChar](NormalModeCommand& com) {
		com.textObject = inputChar;
		com.stage = ParsingStages::WaitingForTargetChar;
	};

	const auto addTargetChar = [inputChar](NormalModeCommand& com) {
		com.targetChar = inputChar;
		com.stage = ParsingStages::Finish;
	};

	const auto clearInputs = [&input, this] {
		input.clear();
		command = NormalModeCommand();
	};

	const bool count1 = parseCount(this->command.count1, inputChar);
	const bool action = parseAction(inputChar);
	const bool operation = parseOperation(inputChar);
	const bool count2 = parseCount(this->command.count2, inputChar);
	const bool motion = parseMotion(inputChar);
	const bool textObject = parseTextObject(inputChar);

	if (this->command.stage == ParsingStages::Count1OperationMotionTextObject) {
		if (count1) {
			addCount(this->command.count1);
		} else if (action) {
			addAction(this->command);
		} else if (operation) {
			addOperation(this->command);
		} else if (motion) {
			addMotion(this->command);
		} else if (textObject) {
			addTextObject(this->command);
		} else {
			clearInputs();
		}
	} else if (this->command.stage == ParsingStages::Count2MotionTextObject) {
		if (count2) {
			addCount(this->command.count2);
		} else if (motion) {
			addMotion(this->command);
		} else if (textObject) {
			addTextObject(this->command);
		} else {
			clearInputs();
		}
	} else if (this->command.stage == ParsingStages::WaitingForTargetChar) {
		addTargetChar(this->command);
	}
}

void NormalMode::HandleKeyboardInput(EditorState& state, const std::reference_wrapper<Document> doc) {
	auto& [text, view, cursor, _] = doc.get();

	parseCommand(state.input_, state.input_.back());

	std::cout << "Parse mode: " << (int)this->command.stage << ". Count1: " << this->command.count1
		  << ", operation: " << this->command.operation << ", count2: " << this->command.count2
		  << ", motion: " << this->command.motion << ", text object: " << this->command.textObject
		  << ", target char: " << this->command.targetChar << '\n';

	if (this->command.stage == ParsingStages::Finish) {
		executeNormalModeCommand(text, cursor, state);
		updateView(view, cursor);

		command = NormalModeCommand();
		state.input_.clear();
	}
}

void NormalMode::executeNormalModeCommand(std::unique_ptr<ITextBuffer>& text, Cursor& cursor, EditorState& state) {
	const auto action = actions.find(command.operation);
	const auto operation = operations.find(command.operation);
	const auto motion = motions.find(command.motion);
	const auto textObject = textObjects.find(command.textObject);

	const auto startRange = MotionRange{.x = cursor.getX(), .y = cursor.getY()};

	for (auto i{0zu}; i < std::max(1, command.count1) * std::max(1, command.count2); ++i) {
		// std::cout << "Called: " << i << '\n';
		if (motion != motions.end()) {
			(this->*motion->second)(text, cursor, state);
		} else if (textObject != textObjects.end()) {
			(this->*textObject->second)(text, cursor, state);
		}
	}

	const auto endRange = MotionRange{.x = cursor.getX(), .y = cursor.getY()};

	if (action != actions.end()) {
		(this->*action->second)(text, cursor, state);
	} else if (operation != operations.end()) {
		(this->*operation->second)(text, cursor, state, startRange, endRange);

		if (command.operation == 'd') {
			cursor.setY(startRange.y);
			cursor.setX(startRange.x);
		}
	}
}

NormalMode::NormalMode() {

	operations = {{'d', &NormalMode::operationDeleteChar}, {'y', &NormalMode::operationCopyText}};

	actions = {
	    {'O', &NormalMode::actionInsertLineAbove},	  {'o', &NormalMode::actionInsertLineBelow},
	    {'i', &NormalMode::actionSwitchToInsertLeft}, {'a', &NormalMode::actionSwitchToInsertRight},
	    {'x', &NormalMode::actionDeleteChar},
	};

	motions = {
	    {'h', &NormalMode::motionMoveCursorLeft},	   {'j', &NormalMode::motionMoveCursorDown},
    {'k', &NormalMode::motionMoveCursorUp},	   {'l', &NormalMode::motionMoveCursorRight},
    {'G', &NormalMode::motionMoveCursorBottomFile}, {'$', &NormalMode::motionMoveRightMost},
    {'0', &NormalMode::motionMoveLeftMost},	   {'^', &NormalMode::motionMoveLeftMostChar},
    {'w', &NormalMode::motionStartOfNextWord},	   {'b', &NormalMode::motionStartOfPrevWord},
    {'W', &NormalMode::motionStartOfNextWORD},	   {'B', &NormalMode::motionStartOfPrevWORD},
	    {'e', &NormalMode::motionEndOfWord}, {'E', &NormalMode::motionEndOfWORD},
	};

	textObjects = {
	    {'f', &NormalMode::findFirstCharRight},
	    {'F', &NormalMode::findFirstCharLeft},
	};
}

void NormalMode::operationDeleteChar(FUNC_TYPES, const MotionRange& start, const MotionRange& end) const {
	if (start.y == end.y) {
		text->deleteRange(start.y, start.x, end.x - start.x);
	} else {

	    /*for (const auto it = text->forwardIterator(cursor.getY()); !it->end(cursor.get); it->next()) {

	    }*/
		// text->deleteCharacter(cursor.getY(), cursor.getX());
	}
}

void NormalMode::operationCopyText(FUNC_TYPES, const MotionRange& start, const MotionRange& end) const {

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

void NormalMode::motionStartOfNextWord(FUNC_TYPES) const {
    const auto currLine = text->rowsView(cursor.getY());
    const size_t startX = cursor.getX();

    if (startX >= currLine.length()) return;

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

void NormalMode::motionStartOfNextWORD(FUNC_TYPES) const {
	const auto spaceSeparator = " "s;
	const auto currLine = text->rowsView(cursor.getY());

	const std::size_t indexPunctuation = currLine.find_first_of(spaceSeparator, cursor.getX());

	if (indexPunctuation != std::string_view::npos) { // first line check
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

void NormalMode::motionStartOfPrevWord(FUNC_TYPES) const {
    const std::string alphabet = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_";
    const std::string space = " \t\r\n";

    int y = cursor.getY();
    int x = cursor.getX();

    if (x > 0) x--;
    else if (y > 0) { y--; x = text->rowsView(y).empty() ? 0 : text->rowsView(y).length() - 1; }
    else return;

    auto currLine = text->rowsView(y);

    while (y >= 0 && (currLine.empty() || isspace(currLine[x]))) {
        if (currLine.empty()) {
            cursor.setX(0);
            cursor.setY(y); return;
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

void NormalMode::motionEndOfWord(FUNC_TYPES) const {
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
    } else return;

    while (y < text->linesCount() && (currLine.empty() || isspace(currLine[x]))) {
        size_t firstVisible = currLine.find_first_not_of(space, x);
        if (firstVisible != std::string_view::npos) {
            x = static_cast<int>(firstVisible);
            break;
        }
        if (++y >= text->linesCount()) return;
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

void NormalMode::motionEndOfWORD(FUNC_TYPES) const {
    int y = cursor.getY();
    int x = cursor.getX();
    auto currLine = text->rowsView(y);
    const std::string space = " \t\r\n";

    // 1. Step Forward
    if (x + 1 < currLine.length()) x++;
    else if (y + 1 < text->linesCount()) {
        y++; x = 0;
        currLine = text->rowsView(y);
    } else return;

    // 2. Skip Whitespace
    while (y < text->linesCount() && (currLine.empty() || isspace(currLine[x]))) {
        size_t firstVisible = currLine.find_first_not_of(space, x);
        if (firstVisible != std::string_view::npos) {
            x = static_cast<int>(firstVisible);
            break;
        }
        if (++y >= text->linesCount()) return;
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

void NormalMode::motionStartOfPrevWORD(FUNC_TYPES) const {
    const auto spaceSeparator = " "s;
    int y = cursor.getY();
    int x = cursor.getX();

    auto currLine = text->rowsView(y);

    if (x == 0 || isspace(currLine[x]) || (!isspace(currLine[x]) && isspace(currLine[x-1]))) {
        if (x == 0) {
            if (y == 0) return; // Top of file
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

    // Step 3: Find the start of this WORD
    size_t startOfWord = currLine.find_last_of(spaceSeparator, x);
    cursor.setY(y);
    cursor.setX(startOfWord == std::string_view::npos ? 0 : startOfWord + 1);
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

void NormalMode::motionMoveCursorLeft(FUNC_TYPES) const {
	if (cursor.getX() > 0) {
		cursor.decrementX();
	}
}

void NormalMode::motionMoveCursorRight(FUNC_TYPES) const {
	if (text->rowsLength(cursor.getY()) - 1 > cursor.getX()) {
		cursor.incrementX();
	}
}

void NormalMode::motionMoveCursorUp(FUNC_TYPES) const {
	if (cursor.getY() > 0) {
		const auto currRowLength = text->rowsLength(cursor.getY());
		cursor.decrementY();
		const auto nextRowLength = text->rowsLength(cursor.getY());

		if (currRowLength - 1 == cursor.getX() || nextRowLength - 1 < cursor.getX()) {
			cursor.setX(nextRowLength - 1);
		}
	}
}

void NormalMode::motionMoveCursorDown(FUNC_TYPES) const {
	if (text->linesCount() - 1 > cursor.getY()) {

		const size_t currRowLength = text->rowsLength(cursor.getY());

		cursor.incrementY();
		const size_t nextRowLength = text->rowsLength(cursor.getY());

		if (currRowLength - 1 == cursor.getX() || nextRowLength - 1 <= cursor.getX()) {
			cursor.setX(nextRowLength - 1);
		}
	}
}

void NormalMode::motionMoveCursorBottomFile(FUNC_TYPES) const {
	cursor.setY(text->linesCount() - 1);
	cursor.setX(std::min(text->rowsLength(cursor.getY()) - 1, cursor.getX()));
}

void NormalMode::motionMoveRightMost(FUNC_TYPES) const {
	cursor.setX(text->rowsLength(cursor.getY()) - 1);
}

void NormalMode::motionMoveLeftMostChar(FUNC_TYPES) const {
	const auto line = text->rowsView(cursor.getY());
	// Assuming 'separators' is accessible via state or text
	const size_t index = line.find_first_not_of(" \t");

	if (index != std::string::npos) {
		cursor.setX(index);
	}
}

void NormalMode::motionMoveLeftMost(FUNC_TYPES) const {
	cursor.setX(0);
}

void NormalMode::actionDeleteChar(FUNC_TYPES) const {}

void NormalMode::actionInsertLineAbove(FUNC_TYPES) const {
	text->insertLine(cursor.getY(), "");
	cursor.setX(0);
	state.currentMode_ = Modes::Insert;
}

void NormalMode::actionInsertLineBelow(FUNC_TYPES) const {
	text->insertLine(cursor.getY() + 1, "");
	cursor.incrementY();
	cursor.setX(0);
	state.currentMode_ = Modes::Insert;
}

void NormalMode::findFirstCharRight(FUNC_TYPES) const {
	const auto reversedView = text->rowSubstr(cursor.getY(), 0, cursor.getX()) | std::views::reverse;
	const auto it = std::ranges::find(reversedView, state.input_.back());

	if (it != reversedView.end()) {
		const auto offset = cursor.getX() - std::distance(reversedView.begin(), it) - 1;
		cursor.setX(offset);
	}
}

void NormalMode::findFirstCharLeft(FUNC_TYPES) const {
	const auto subView = text->rowSubstr(cursor.getY(), cursor.getX() + 1);
	const auto it = std::ranges::find(subView, state.input_.back());

	if (it != subView.end()) {
		cursor.setX(cursor.getX() + 1 + std::distance(subView.begin(), it));
	}
}

void NormalMode::actionSwitchToInsertLeft(FUNC_TYPES) const {
	state.currentMode_ = Modes::Insert;
}

void NormalMode::actionSwitchToInsertRight(FUNC_TYPES) const {
	state.currentMode_ = Modes::Insert;
	if (cursor.getX() <= text->rowsLength(cursor.getY()) - 1)
		cursor.incrementX();
}