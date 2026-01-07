#include "commands/NormalMode.hpp"

#include <algorithm>
#include <iostream>
#include <ranges>

#include "core/Editor.hpp"
#include "buffer/Files.hpp"

NormalMode::NormalMode(): paramFunc_{nullptr}, paramCount_{} {
    paramCommands_ = {
           {"f", [] { return &NormalMode::findFirstCharRight; }},
           {"F", []{ return &NormalMode::findFirstCharLeft; }},
    };

    fixedCommands_ = {
            {"h", [] { return &NormalMode::moveCursorLeft; }},
            {"j", [] { return &NormalMode::moveCursorDown; }},
            {"k", [] { return &NormalMode::moveCursorUp; }},
            {"l", [] { return &NormalMode::moveCursorRight; }},
            {"gg", [] { return &NormalMode::moveCursorTopFile; }},
            {"G", [this](EditorState& state, Document& doc) { moveCursorBottomFile(state, doc); }},
            {"$", [this](EditorState& state, Document& doc) { moveRightMost(state, doc); }},
            {"0", [this](EditorState& state, Document& doc) { moveLeftMost(state, doc); }},
            {"^", [this](EditorState& state, Document& doc) { moveLeftMostChar(state, doc); }},
            {"dd", [this](EditorState& state, Document& doc) { deleteLine(state, doc); }},
            {"dw", [this](EditorState& state, Document& doc) { deleteWord(state, doc); }},
            {"daw", [this](EditorState& state, Document& doc) { deleteAllWord(state, doc); }},
            {"x", [this](EditorState& state, Document& doc) { deleteChar(state, doc);}},
            {"O", [this](EditorState& state, Document& doc) { insertLineAbove(state, doc); }},
            {"o", [this](EditorState& state, Document& doc) { insertLineBelow(state, doc); }},
            {"i", [this](EditorState& state, Document& doc) { switchToInsertLeft(state, doc); }},
            {"a", [this](EditorState& state, Document& doc) { switchToInsertRight(state, doc); }},
    };
}

void NormalMode::moveCursorLeft(EditorState&, Document& doc) {
    if (doc.cursor_.getX() > 0) {
        doc.cursor_.decrementX();
    }
}

void NormalMode::moveCursorRight(EditorState&, Document& doc) {
    if (doc.textBuffer_->rowsLength(doc.cursor_.getY()) - 1 > doc.cursor_.getX()) {
        doc.cursor_.incrementX();
    }
}

void NormalMode::moveCursorUp(EditorState&, Document& doc) {
    if (doc.cursor_.getY() > 0) {
        const auto currRowLength = doc.textBuffer_->rowsLength(doc.cursor_.getY());

    	doc.cursor_.decrementY();

        const auto nextRowLength = doc.textBuffer_->rowsLength(doc.cursor_.getY());

        if (currRowLength - 1 == doc.cursor_.getX() || nextRowLength - 1 <= doc.cursor_.getX()) {
            doc.cursor_.setX(nextRowLength - 1);
        }
    }
}

void NormalMode::moveCursorDown(EditorState&, Document& doc) {
    if (doc.textBuffer_->linesCount() - 1 > doc.cursor_.getY()) {
        const size_t currRowLength = doc.textBuffer_->rowsLength(doc.cursor_.getY());

    	doc.cursor_.incrementY();

        const size_t nextRowLength = doc.textBuffer_->rowsLength(doc.cursor_.getY());

        if (currRowLength - 1 == doc.cursor_.getX() || nextRowLength - 1 <= doc.cursor_.getX()) {
            doc.cursor_.setX(nextRowLength - 1);
        }
    }
}

void NormalMode::moveCursorTopFile(EditorState&, Document& doc) {
    doc.cursor_.setY(0);
    doc.cursor_.setX(std::min(doc.textBuffer_->rowsLength(doc.cursor_.getY()) - 1, doc.cursor_.getX()));
}

void NormalMode::moveCursorBottomFile(EditorState&, Document& doc) {
    doc.cursor_.setY(doc.textBuffer_->linesCount() - 1);
    doc.cursor_.setX(std::min(doc.textBuffer_->rowsLength(doc.cursor_.getY()) - 1, doc.cursor_.getX()));
}

void NormalMode::moveRightMost(EditorState&, Document& doc) {
    doc.cursor_.setX(doc.textBuffer_->rowsLength(doc.cursor_.getY()) - 1);
}

void NormalMode::moveLeftMostChar(EditorState&, Document& doc) {
    const auto line = doc.textBuffer_->rowsView(doc.cursor_.getY());
    const size_t index = line.find_first_not_of(doc.separators);

    if (index != std::string::npos) {
        doc.cursor_.setX(index);
    }
}

void NormalMode::moveLeftMost(EditorState&, Document& doc) {
    doc.cursor_.setX(0);
}

void NormalMode::deleteLine(EditorState&, Document& doc) {
    //fix this
    if (doc.textBuffer_->linesCount() == 1 && doc.textBuffer_->rowsLength(0) == 0) {
        return;
    }

    doc.textBuffer_->deleteLine(doc.cursor_.getY());

    doc.cursor_.setY(std::min(doc.textBuffer_->linesCount() - 1, doc.cursor_.getY()));
    doc.cursor_.setX(std::min(doc.textBuffer_->rowsLength(doc.cursor_.getY()) - 1, doc.cursor_.getX()));
}

void NormalMode::deleteChar(EditorState&, Document& doc) {
	doc.textBuffer_->deleteCharacter(doc.cursor_.getY(), doc.cursor_.getX());
}

void NormalMode::deleteWord(EditorState&, Document& doc) {
    const auto index = doc.textBuffer_->rowSubstr(doc.cursor_.getY(), doc.cursor_.getX()).find_first_of(doc.separators);
    doc.textBuffer_->deleteRange(doc.cursor_.getY(), doc.cursor_.getX(), doc.cursor_.getX() + index);
}

void NormalMode::deleteAllWord(EditorState&, Document& doc) {
    const auto firstPart = doc.textBuffer_->rowSubstr(doc.cursor_.getY(), 0, doc.cursor_.getX());
    const size_t lastSep = firstPart.find_last_of(doc.textBuffer_->separators_);
    const size_t prevIndex = lastSep == std::string_view::npos ? 0 : lastSep + 1;

    const auto secondPart= doc.textBuffer_->rowSubstr(doc.cursor_.getY(), doc.cursor_.getX());
    const auto relativeNext = secondPart.find_first_of(doc.textBuffer_->separators_);
    const size_t nextIndex = relativeNext == std::string_view::npos ? prevIndex: doc.cursor_.getX() + relativeNext;

    doc.textBuffer_->deleteRange(doc.cursor_.getY(), prevIndex, nextIndex - prevIndex);
}

void NormalMode::insertLineAbove(EditorState& editorState, Document& doc) {
	doc.textBuffer_->insertLine(doc.cursor_.getY());
	doc.cursor_.setX(0);
	editorState.currentMode_ = Modes::Insert;
}

void NormalMode::insertLineBelow(EditorState& editorState, Document& doc) {
	doc.textBuffer_->insertLine(doc.cursor_.getY() + 1);
	doc.cursor_.incrementY();
	doc.cursor_.setX(0);

	editorState.currentMode_ = Modes::Insert;
}

void NormalMode::findFirstCharRight(EditorState& editorState, Document& doc) {
    const auto reversedView = doc.textBuffer_->rowSubstr(doc.cursor_.getY(), 0, doc.cursor_.getX()) | std::views::reverse;
    const auto index = std::ranges::find(reversedView, editorState.input_.back());

    if (index != reversedView.end()) {
        const auto offset = doc.cursor_.getX() - std::distance(reversedView.begin(), index) - 1;
        doc.cursor_.setX(offset);
    }
}

void NormalMode::findFirstCharLeft(EditorState& editorState, Document& doc) {
    const auto subView = doc.textBuffer_->rowSubstr(doc.cursor_.getY(), doc.cursor_.getX() + 1);
    const auto index = std::ranges::find(subView, editorState.input_.back());

    if (index != subView.end()) {
        doc.cursor_.setX(std::distance(doc.textBuffer_->rowsView(doc.cursor_.getY()).begin(), index));
    }
}

void NormalMode::switchToInsertLeft(EditorState& editorState, Document&) {
	editorState.currentMode_ = Modes::Insert;
}
void NormalMode::switchToInsertRight(EditorState& editorState, Document& doc) {
	editorState.currentMode_ = Modes::Insert;
	this->moveCursorRight(editorState, doc);
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

void NormalMode::HandleKeyboardInput(EditorState& editorState, Document& document) {
    auto& [text, view, cursor, _] = document;

	if (paramFunc_ != nullptr && paramCount_ + 1 == editorState.input_.size()) {
		paramFunc_(editorState, document);
		editorState.input_.clear();
		paramFunc_ = nullptr;
		paramCount_ = 0;
	    this->updateView(editorState, document);

	} else if (const auto it = paramCommands_.find(editorState.input_); it != paramCommands_.end()) {
		paramFunc_ = it->second;
		paramCount_ = editorState.input_.size();

	} else if (const auto it = fixedCommands_.find(editorState.input_); it != fixedCommands_.end()) {
		it->second(editorState, document);
		editorState.input_.clear();
		paramFunc_ = nullptr;
	    this->updateView(editorState, document);

	} else {
		const bool flag1 = std::ranges::any_of(paramCommands_, [&](const auto& c) {return c.first.starts_with(editorState.input_);});
		const bool flag2 = std::ranges::any_of(fixedCommands_, [&](const auto& c) {return c.first.starts_with(editorState.input_);});

		if (!flag1 && !flag2) {
			editorState.input_.clear();
			paramFunc_ = nullptr;
		}
	}
}