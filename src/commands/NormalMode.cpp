#include "commands/NormalMode.hpp"

#include <algorithm>
#include <iostream>
#include <ranges>

#include "core/Editor.hpp"
#include "buffer/Files.hpp"

Command::Command(): state{WaitCount || WaitOperation || WaitMotion}, count{1} {}

bool NormalMode::parseCount(Command& com, const std::string_view input) {
    if (std::isdigit(input.back())) {
        if (input.back() == '0') {
            return false;
        }
        com.count = com.count * 10 + input.back() - '0';

    }

    return false;
}

bool NormalMode::parseOperation(Command& com, std::string_view input) {
    if (com.operation.empty()) {
        if (const auto it = operations_.find(input.substr(input.length() - 1)); it != operations_.end()) {
            com.operation = std::string(input.substr(input.length() - 1));
            return true;
        }

        return false;
    } else {

    }
    return false;
}

bool NormalMode::parseMotion(Command& com, std::string_view input) {
    return false;
}



void NormalMode::HandleKeyboardInput(EditorState& editorState, Document& document) {

    if ((command_.state | WaitCount) != 0) {

    }
    if ((command_.state | WaitOperation) != 0) {

    }
    if ((command_.state | WaitMotion) != 0) {

    }

    /*auto& [text, view, cursor, _] = document;

    if (paramFunc_ != nullptr && paramCount_ + 1 == editorState.input_.size()) {
        auto func = paramFunc_;
        (this->*func)(document.textBuffer_, document.cursor_, editorState);
        editorState.input_.clear();
        paramFunc_ = nullptr;
        paramCount_ = 0;
        this->updateView(view, cursor);

    } else if (const auto it = paramCommands_.find(editorState.input_); it != paramCommands_.end()) {
        paramFunc_ = it->second;
        paramCount_ = editorState.input_.size();

    } else if (const auto it = fixedCommands_.find(editorState.input_); it != fixedCommands_.end()) {
        auto func = it->second;
        (this->*func)(document.textBuffer_, document.cursor_, editorState);
        editorState.input_.clear();
        paramFunc_ = nullptr;
        this->updateView(view, cursor);

    } else {
        const bool flag1 = std::ranges::any_of(paramCommands_, [&](const auto& c) {return c.first.starts_with(editorState.input_);});
        const bool flag2 = std::ranges::any_of(fixedCommands_, [&](const auto& c) {return c.first.starts_with(editorState.input_);});

        if (!flag1 && !flag2) {
            editorState.input_.clear();
            paramFunc_ = nullptr;
        }
    }*/
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

NormalMode::NormalMode() : paramFunc_{nullptr}, paramCount_{0} {
    operations_ = {
        {"x", &NormalMode::deleteChar},
    };

    motions_ = {
        {"h",   &NormalMode::moveCursorLeft},
        {"j",   &NormalMode::moveCursorDown},
        {"k",   &NormalMode::moveCursorUp},
        {"l",   &NormalMode::moveCursorRight},
        {"0", &NormalMode::moveLeftMost}
    };

    /*paramCommands_ = {
        {"f", &NormalMode::findFirstCharRight},
        {"F", &NormalMode::findFirstCharLeft}
    };

    fixedCommands_ = {
        {"h",   &NormalMode::moveCursorLeft},
        {"j",   &NormalMode::moveCursorDown},
        {"k",   &NormalMode::moveCursorUp},
        {"l",   &NormalMode::moveCursorRight},
        {"gg",  &NormalMode::moveCursorTopFile},
        {"G",   &NormalMode::moveCursorBottomFile},
        {"$",   &NormalMode::moveRightMost},
        {"0",   &NormalMode::moveLeftMost},
        {"^",   &NormalMode::moveLeftMostChar},
        {"dd",  &NormalMode::deleteLine},
        {"dw",  &NormalMode::deleteWord},
        {"daw", &NormalMode::deleteAllWord},
        {"x",   &NormalMode::deleteChar},
        {"O",   &NormalMode::insertLineAbove},
        {"o",   &NormalMode::insertLineBelow},
        {"i",   &NormalMode::switchToInsertLeft},
        {"a",   &NormalMode::switchToInsertRight}
    };*/
}

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

void NormalMode::deleteChar(FUNC_TYPES) {
    text->deleteCharacter(cursor.getY(), cursor.getX());
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
    const size_t nextIndex = relativeNext == std::string_view::npos ? text->rowsLength(cursor.getY()) : cursor.getX() + relativeNext;

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
