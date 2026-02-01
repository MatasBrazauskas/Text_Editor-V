#include "commands/NormalMode.hpp"

#include "buffer/Files.hpp"
#include "core/Editor.hpp"

#include <algorithm>
#include <iostream>
#include <ranges>
#include <SDL_clipboard.h>

static bool parseCount(const int count, const char inputChar) {
	if (!std::isdigit(inputChar)) return false;

	if (inputChar == '0' && count == 0) return false;

	return true;
}

static bool parseOperation(const std::unordered_map<char, Func2>& map, const char inputChar) {
	return map.contains(inputChar);
}

static bool parseMotion(const std::unordered_map<char, Func>& map, const char inputChar) {
    return map.contains(inputChar);
}

static bool parseTextObject(const std::unordered_map<char, Func>& map, const char inputChar) {
    return map.contains(inputChar);
}

NormalModeCommand::NormalModeCommand(): count1{0}, operation{' '}, count2{0}, motion{' '}, textObject{' '}, targetChar{' '}, ignoreCount{}, stage{ParsingStages::Count1OperationMotionTextObject} {}

NormalModeCommand::NormalModeCommand(int count1, char operation, int count2, char motion, char textObject, char targetChar, bool ignoreCount, ParsingStages stage)
    : count1{count1}, operation{operation}, count2{count2}, motion{motion}, textObject{textObject}, targetChar{targetChar}, ignoreCount{ignoreCount}, stage{stage} {}

void NormalMode::parseCommand(std::string& input, const char inputChar) {
    auto addCount = [inputChar](int& count) {
        count *= 10;
        count += inputChar - '0';
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
    const bool operation = parseOperation(this->operations,inputChar);
    const bool count2= parseCount(this->command.count2, inputChar);
    const bool motion = parseMotion(this->motions, inputChar);
    const bool textObject = parseTextObject(this->textObjects, inputChar);

    if (this->command.stage == ParsingStages::Count1OperationMotionTextObject) {
        if (count1) {
            addCount(this->command.count1);
        }
        else if (operation) {
            addOperation(this->command);
        }
        else if (motion) {
            addMotion(this->command);
        }
        else if (textObject) {
            addTextObject(this->command);
        } else {
            clearInputs();
        }
    } else if (this->command.stage == ParsingStages::Count2MotionTextObject) {
        if (count2) {
            addCount(this->command.count2);
        } else if (motion) {
            addMotion(this->command);
        }else if (textObject) {
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
	std::cout << std::boolalpha;

    parseCommand(state.input_, state.input_.back());

    std::cout << "Parse mode: " << (int)this->command.stage << ". Count1: " << this->command.count1
          << ", operation: " << this->command.operation << ", count2: " << this->command.count2
          << ", motion: " << this->command.motion << ", text object: " << this->command.textObject
            << ", target char: " << this->command.targetChar << '\n';

    /*if (this->command.stage == ParsingStages::Finish) {
        executeNormalModeCommand(text, cursor, state);
        updateView(view, cursor);

        command = NormalModeCommand();
        state.input_.clear();
    }*/
}

void NormalMode::executeNormalModeCommand(std::unique_ptr<ITextBuffer>& text, Cursor& cursor, EditorState& state) {

	const auto operation = operations.find(command.operation);
	const auto motion = motions.find(command.motion);
	const auto textObject = textObjects.find(command.textObject);

	const auto startRange = MotionRange{.x = cursor.getX(), .y = cursor.getY()};

	for (auto i{0zu}; i < std::max(1, command.count1) * std::max(1, command.count2); ++i) {
		std::cout << "Called: " << i << '\n';
		if (motion != motions.end()) {
			(this->*motion->second)(text, cursor, state);
		} else if (textObject != textObjects.end()) {
			(this->*textObject->second)(text, cursor, state);
		}
	}

	const auto endRange = MotionRange{.x = cursor.getX(), .y = cursor.getY()};

    if (operation != operations.end()) {
        (this->*operation->second)(text, cursor, state, startRange, endRange);

        cursor.setY(startRange.y);
        cursor.setX(startRange.x);
    }
}

NormalMode::NormalMode() {

	operations = {
	    {'d', &NormalMode::operationDeleteChar},
	    {'y', &NormalMode::operationCopyText}
	};

	motions = {
	    {'h', &NormalMode::motionMoveCursorLeft},
	    {'j', &NormalMode::motionMoveCursorDown},
	    {'k', &NormalMode::motionMoveCursorUp},
	    {'l', &NormalMode::motionMoveCursorRight},
	    {'G', &NormalMode::motionMoveCursorBottomFile},
	    {'$', &NormalMode::motionMoveRightMost},
	    {'0', &NormalMode::motionMoveLeftMost},
	    {'^', &NormalMode::motionMoveLeftMostChar},
        {'x', &NormalMode::motionDeleteChar},
	};

	textObjects = {
	    {'f', &NormalMode::findFirstCharRight},
	    {'F', &NormalMode::findFirstCharLeft},
	};

	/*fixedCommands_ = {
	    {"O", &NormalMode::insertLineAbove},
	    {"o", &NormalMode::insertLineBelow},    {"i", &NormalMode::switchToInsertLeft},
	    {"a", &NormalMode::switchToInsertRight}
	};*/
}

void NormalMode::operationDeleteChar(FUNC_TYPES, const MotionRange& start, const MotionRange& end) const {
    if (start.y == end.y) {
        text->deleteRange(start.y, start.x, end.x - start.x + 1);
    } else {
        // text->deleteCharacter(cursor.getY(), cursor.getX());
    }
}

void NormalMode::operationCopyText(FUNC_TYPES, const MotionRange& start, const MotionRange& end) const {
    if (start.y == end.y) {
        const auto subString = text->rowsView(start.y);
        std::cout << "Copies test: " << subString << '\n';
        if (SDL_SetClipboardText(std::string(subString.data()).c_str()) != 0) {
            throw std::runtime_error(SDL_GetError());
        }
    } else {
        // text->deleteCharacter(cursor.getY(), cursor.getX());
    }
}

void NormalMode::updateView(TextBufferView& view, const Cursor& cursor) {
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

void NormalMode::motionDeleteChar(FUNC_TYPES) const {
}

void NormalMode::insertLineAbove(FUNC_TYPES) {
	text->insertLine(cursor.getY());
	cursor.setX(0);
	state.currentMode_ = Modes::Insert;
}

void NormalMode::insertLineBelow(FUNC_TYPES) {
	text->insertLine(cursor.getY() + 1);
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

void NormalMode::switchToInsertLeft(FUNC_TYPES) {
	state.currentMode_ = Modes::Insert;
}

void NormalMode::switchToInsertRight(FUNC_TYPES) {
	state.currentMode_ = Modes::Insert;
	if (cursor.getX() <= text->rowsLength(cursor.getY()) - 1)
		cursor.incrementX();
}
