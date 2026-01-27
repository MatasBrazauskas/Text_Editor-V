#include "commands/NormalMode.hpp"

#include "buffer/Files.hpp"
#include "core/Editor.hpp"

#include <algorithm>
#include <iostream>
#include <ranges>

static bool parseCount1(NormalModeCommand& com, const char inputChar) {
	if (std::isdigit(inputChar) == false)
		return false;

	if (inputChar == '0' && com.count1 == 0) {
		return false;
	}

	com.count1 *= 10;
	com.count1 += inputChar - '0';
	return true;
}

bool parseCount2(NormalModeCommand& com, const char inputChar) {
	if (std::isdigit(inputChar) == false)
		return false;

	if (inputChar == '0' && com.count2 == 0) {
		return false;
	}

	com.count2 *= 10;
	com.count2 += inputChar - '0';
	return true;
}

bool NormalMode::parseOperation(NormalModeCommand& com, const char inputChar) {
    const auto it = this->operations.find(std::string{inputChar});
	if (it != this->operations.end()) {
		com.operation = std::string{inputChar};
		return true;
	}
	return false;
}

bool NormalMode::parseMotion(NormalModeCommand& com, const char inputChar) {
    const auto it = this->motions.find(std::string{inputChar});
	if (it != this->motions.end()) {
		com.motion = std::string{inputChar};
		return true;
	}
	return false;
}

bool NormalMode::parseTextObject(NormalModeCommand& com, const char inputChar) {
	const auto it = this->textObjects.find(std::string{inputChar});
	if (it != this->textObjects.end()) {
		com.textObject = std::string{inputChar};
		return true;
	}
	return false;
}

NormalModeCommand::NormalModeCommand() : count1{0}, count2{0}, stage{ParsingStages::Count1OperationMotionTextObject} {}

void NormalMode::HandleKeyboardInput(EditorState& state, std::reference_wrapper<Document> doc) {
	auto& [text, view, cursor, _] = doc.get();
    std::cout << std::boolalpha;

	if (this->command.stage == ParsingStages::Count1OperationMotionTextObject) {
		bool first = parseCount1(this->command, state.input_.back());

		if (first) {
			// dont do anythink
			std::cout << "Digit\n";
		} else {
			bool second = parseOperation(this->command, state.input_.back());

			if (second) {
				this->command.stage = ParsingStages::Count2MotionTextObject;
			} else {
				bool third = parseMotion(this->command, state.input_.back());
				if (third == true) {
					this->command.stage = ParsingStages::Finish;
				} else {
					bool fourth = parseTextObject(this->command, state.input_.back());
					if (fourth) {
						this->command.stage = ParsingStages::TextObject;
					}
				}
			}
		}
	} else if (this->command.stage == ParsingStages::Count2MotionTextObject) {
		bool first = parseCount2(this->command, state.input_.back());

		if (first) {

		} else {
			bool second = parseMotion(this->command, state.input_.back());
			if (second) {
				this->command.stage = ParsingStages::Finish;
			} else {
				bool third = parseTextObject(this->command, state.input_.back());
				if (third) {
					this->command.stage = ParsingStages::TextObject;
				}
			}
		}

	} else if (this->command.stage == ParsingStages::TextObject) {
		this->command.textObject.push_back(state.input_.back());
		this->command.stage = ParsingStages::Finish;
	} else {
	    this->command = NormalModeCommand();
	    state.input_.clear();
	}
    if (this->command.stage == ParsingStages::Finish) {
        std::cout << "Command finished\n";
        executeNormalModeCommand(text, cursor, state);

        this->updateView(view, cursor);

        this->command = NormalModeCommand();
        state.input_.clear();
    }

	std::cout << "Parse mode: " << (int)this->command.stage << ". Count1: " << this->command.count1
		  << ", operation: " << this->command.operation << ", count2: " << this->command.count2
		  << ", motion: " << this->command.motion << ", text object: " << this->command.textObject << '\n';
}

void NormalMode::executeNormalModeCommand(std::unique_ptr<ITextBuffer>&text, Cursor &cursor, EditorState &state) {

    const auto operation = operations.find(command.operation);
    const auto motion = motions.find(command.motion);
    const auto textObject = motions.find(command.textObject);

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

    (this->*operation->second)(text, cursor, state, startRange, endRange);

    cursor.setY(startRange.y);
    cursor.setX(startRange.x);
}

NormalMode::NormalMode() : paramFunc_{nullptr}, paramCount_{0} {

	operations = {
	    {"d", &NormalMode::deleteChar},
	};

	motions = {
	    {"h", &NormalMode::moveCursorLeft},
	    {"j", &NormalMode::moveCursorDown},
	    {"k", &NormalMode::moveCursorUp},
	    {"l", &NormalMode::moveCursorRight},
	};

	textObjects = {
	    {"f", &NormalMode::findFirstCharRight},
	    {"F", &NormalMode::findFirstCharLeft},
	};

	/*paramCommands_ = {{"f", &NormalMode::findFirstCharRight},
		  {"F", &NormalMode::findFirstCharLeft}};

	fixedCommands_ = {
	    {"h", &NormalMode::moveCursorLeft},	    {"j", &NormalMode::moveCursorDown},
	    {"k", &NormalMode::moveCursorUp},	    {"l", &NormalMode::moveCursorRight},
	    {"gg", &NormalMode::moveCursorTopFile}, {"G", &NormalMode::moveCursorBottomFile},
	    {"$", &NormalMode::moveRightMost},	    {"0", &NormalMode::moveLeftMost},
	    {"^", &NormalMode::moveLeftMostChar},   {"dd", &NormalMode::deleteLine},
	    {"dw", &NormalMode::deleteWord},	    {"daw", &NormalMode::deleteAllWord},
	    {"x", &NormalMode::deleteChar},	    {"O", &NormalMode::insertLineAbove},
	    {"o", &NormalMode::insertLineBelow},    {"i", &NormalMode::switchToInsertLeft},
	    {"a", &NormalMode::switchToInsertRight}
	};*/
}

void NormalMode::currentLine(std::unique_ptr<ITextBuffer>& text, Cursor& cursor, EditorState& state) {}

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

/*NormalMode::NormalMode() : paramFunc_{nullptr}, paramCount_{0} {
	paramCommands_ = {{"f", &NormalMode::findFirstCharRight},
			  {"F", &NormalMode::findFirstCharLeft}};

	fixedCommands_ = {
	    {"h", &NormalMode::moveCursorLeft},	    {"j", &NormalMode::moveCursorDown},
	    {"k", &NormalMode::moveCursorUp},	    {"l", &NormalMode::moveCursorRight},
	    {"gg", &NormalMode::moveCursorTopFile}, {"G", &NormalMode::moveCursorBottomFile},
	    {"$", &NormalMode::moveRightMost},	    {"0", &NormalMode::moveLeftMost},
	    {"^", &NormalMode::moveLeftMostChar},   {"dd", &NormalMode::deleteLine},
	    {"dw", &NormalMode::deleteWord},	    {"daw", &NormalMode::deleteAllWord},
	    {"x", &NormalMode::deleteChar},	    {"O", &NormalMode::insertLineAbove},
	    {"o", &NormalMode::insertLineBelow},    {"i", &NormalMode::switchToInsertLeft},
	    {"a", &NormalMode::switchToInsertRight}};
}*/

void NormalMode::moveCursorLeft(FUNC_TYPES) {
	if (cursor.getX() > 0) {
		cursor.decrementX();
	}
}

void NormalMode::moveCursorRight(FUNC_TYPES) {
	if (text->rowsLength(cursor.getY()) - 1 > cursor.getX()) {
		cursor.incrementX();
	}
}

void NormalMode::moveCursorUp(FUNC_TYPES) {
	if (cursor.getY() > 0) {
		const auto currRowLength = text->rowsLength(cursor.getY());
		cursor.decrementY();
		const auto nextRowLength = text->rowsLength(cursor.getY());

		if (currRowLength - 1 == cursor.getX() || nextRowLength - 1 < cursor.getX()) {
			cursor.setX(nextRowLength - 1);
		}
	}
}

void NormalMode::moveCursorDown(FUNC_TYPES) {
	if (text->linesCount() - 1 > cursor.getY()) {

		const size_t currRowLength = text->rowsLength(cursor.getY());

		// if (currRowLength > state.)

		cursor.incrementY();
		const size_t nextRowLength = text->rowsLength(cursor.getY());

		if (currRowLength - 1 == cursor.getX() || nextRowLength - 1 <= cursor.getX()) {
			cursor.setX(nextRowLength - 1);
		}
	}
}

void NormalMode::moveCursorTopFile(FUNC_TYPES) {
	cursor.setY(0);
	cursor.setX(std::min(text->rowsLength(cursor.getY()) - 1, cursor.getX()));
}

void NormalMode::moveCursorBottomFile(FUNC_TYPES) {
	cursor.setY(text->linesCount() - 1);
	cursor.setX(std::min(text->rowsLength(cursor.getY()) - 1, cursor.getX()));
}

void NormalMode::moveRightMost(FUNC_TYPES) {
	cursor.setX(text->rowsLength(cursor.getY()) - 1);
}

void NormalMode::moveLeftMostChar(FUNC_TYPES) {
	const auto line = text->rowsView(cursor.getY());
	// Assuming 'separators' is accessible via state or text
	const size_t index = line.find_first_not_of(" \t");

	if (index != std::string::npos) {
		cursor.setX(index);
	}
}

void NormalMode::moveLeftMost(FUNC_TYPES) {
	cursor.setX(0);
}

void NormalMode::deleteLine(FUNC_TYPES) {
	if (text->linesCount() == 1 && text->rowsLength(0) == 0) {
		return;
	}

	text->deleteLine(cursor.getY());
	cursor.setY(std::min(text->linesCount() - 1, cursor.getY()));
	cursor.setX(std::min(text->rowsLength(cursor.getY()) - 1, cursor.getX()));
}

void NormalMode::deleteChar(FUNC_TYPES, const MotionRange& start, const MotionRange& end) {
    if (start.y == end.y) {
        text->deleteRange(start.y, start.x, end.x - start.x + 1);
    }else {
        //text->deleteCharacter(cursor.getY(), cursor.getX());
    }
}

void NormalMode::deleteWord(FUNC_TYPES) {
	const auto index = text->rowSubstr(cursor.getY(), cursor.getX()).find_first_of(" \t");
	text->deleteRange(cursor.getY(), cursor.getX(), index);
}

void NormalMode::deleteAllWord(FUNC_TYPES) {
	const auto firstPart = text->rowSubstr(cursor.getY(), 0, cursor.getX());
	const size_t lastSep = firstPart.find_last_of(" \t");
	const size_t prevIndex = lastSep == std::string_view::npos ? 0 : lastSep + 1;

	const auto secondPart = text->rowSubstr(cursor.getY(), cursor.getX());
	const auto relativeNext = secondPart.find_first_of(" \t");
	const size_t nextIndex =
	    relativeNext == std::string_view::npos ? text->rowsLength(cursor.getY()) : cursor.getX() + relativeNext;

	text->deleteRange(cursor.getY(), prevIndex, nextIndex - prevIndex);
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

void NormalMode::findFirstCharRight(FUNC_TYPES) {
	const auto reversedView = text->rowSubstr(cursor.getY(), 0, cursor.getX()) | std::views::reverse;
	const auto it = std::ranges::find(reversedView, state.input_.back());

	if (it != reversedView.end()) {
		const auto offset = cursor.getX() - std::distance(reversedView.begin(), it) - 1;
		cursor.setX(offset);
	}
}

void NormalMode::findFirstCharLeft(FUNC_TYPES) {
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
