#include "commands/NormalMode.hpp"

#include "buffer/Files.hpp"
#include "core/Editor.hpp"

#include <algorithm>
#include <iostream>

NormalMode::NormalMode(): paramFunc_{nullptr}, paramCount_{} {
    paramCommands_ = {
           {"f", [this](EditorState& state, Document& doc) { return findFirstCharRight(state, doc); }},
           {"F", [this](EditorState& state, Document& doc) { return findFirstCharLeft(state, doc); }},
    };

    fixedCommands_ = {
            {"h", [this](EditorState& state, Document& doc) { moveCursorLeft(state, doc); }},
            {"j", [this](EditorState& state, Document& doc) { moveCursorDown(state, doc); }},
            {"k", [this](EditorState& state, Document& doc) { moveCursorUp(state, doc); }},
            {"l", [this](EditorState& state, Document& doc) { moveCursorRight(state, doc); }},
            {"gg", [this](EditorState& state, Document& doc) { moveCursorTopFile(state, doc); }},
            {"G", [this](EditorState& state, Document& doc) { moveCursorBottomFile(state, doc); }},
            {"$", [this](EditorState& state, Document& doc) { moveRightMost(state, doc); }},
            {"0", [this](EditorState& state, Document& doc) { moveLeftMost(state, doc); }},
            {"^", [this](EditorState& state, Document& doc) { moveRightMostChar(state, doc); }},
            {"dd", [this](EditorState& state, Document& doc) { deleteLine(state, doc); }},
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

        if (doc.textView_.startX_ > doc.cursor_.getX()) {
            doc.textView_.startX_--;
        }
    }
}

void NormalMode::moveCursorRight(EditorState&, Document& doc) {
    if (doc.textBuffer_->rowView(doc.cursor_.getY()).length() - 1 > doc.cursor_.getX()) {
        doc.cursor_.incrementX();

        if (doc.textView_.startX_ + doc.textView_.visibleColumns_ <= doc.cursor_.getX()) {
            doc.textView_.startX_++;
        }
    }
}

void NormalMode::moveCursorUp(EditorState&, Document& doc) {
    if (doc.cursor_.getY() > 0) {
        const size_t currRowLength = doc.textBuffer_->rowView(doc.cursor_.getY()).length();
    	doc.cursor_.decrementY();
        const size_t nextRowLength = doc.textBuffer_->rowView(doc.cursor_.getY()).length();

        if (currRowLength - 1 == doc.cursor_.getX() || nextRowLength - 1 <= doc.cursor_.getX()) {
            doc.cursor_.setX(nextRowLength - 1);
        }
    }
    if (doc.textView_.startY_ > doc.cursor_.getY()) {
        doc.textView_.startY_--;
    }
}

void NormalMode::moveCursorDown(EditorState&, Document& doc) {
    if (doc.textBuffer_->size() - 1 > doc.cursor_.getY()) {
        const size_t currRowLength = doc.textBuffer_->rowView(doc.cursor_.getY()).length();
    	doc.cursor_.incrementY();
        const size_t nextRowLength = doc.textBuffer_->rowView(doc.cursor_.getY()).length();

        if (currRowLength - 1 == doc.cursor_.getX() || nextRowLength - 1 <= doc.cursor_.getX()) {
            doc.cursor_.setX(nextRowLength - 1);
        }
    }
    if (doc.textView_.startY_ + doc.textView_.visibleLines_ <= doc.cursor_.getY()) {
        doc.textView_.startY_++;
    }
}

void NormalMode::moveCursorTopFile(EditorState&, Document& doc) {
    doc.cursor_.setY(0);
    doc.cursor_.setX(std::min(doc.textBuffer_->rowLength(doc.cursor_.getY()) - 1, doc.cursor_.getX()));

    doc.textView_.startY_ = 0;
    doc.textView_.startX_ = std::min(0zu, doc.cursor_.getX() - doc.textView_.visibleColumns_);
}

void NormalMode::moveCursorBottomFile(EditorState&, Document& doc) {
    doc.cursor_.setY(doc.textBuffer_->size() - 1);
    doc.cursor_.setX(std::min(doc.textBuffer_->rowLength(doc.cursor_.getY()) - 1, doc.cursor_.getX()));

    doc.textView_.startY_ = doc.textBuffer_->size() - doc.textView_.visibleLines_;
    doc.textView_.startX_ = std::min(0zu, doc.cursor_.getX() - doc.textView_.visibleColumns_);
}

void NormalMode::moveRightMost(EditorState&, Document& doc) {
    doc.cursor_.setX(doc.textBuffer_->rowView(doc.cursor_.getY()).length() - 1);
    doc.textView_.startX_ = doc.cursor_.getX() - doc.textView_.visibleColumns_ + 1;
}

void NormalMode::moveRightMostChar(EditorState&, Document& doc) {
    const auto line = doc.textBuffer_->rowView(doc.cursor_.getY());
    const size_t index = line.find_first_not_of(doc.separators);

    if (index != std::string::npos) {
        doc.cursor_.setX(index);
        doc.textView_.startX_ = std::min(0zu, doc.cursor_.getX() - doc.textView_.visibleColumns_);
    }
}

void NormalMode::moveLeftMost(EditorState&, Document& doc) {
    doc.cursor_.setX(0);
    doc.textView_.startX_ = 0;
}

void NormalMode::deleteLine(EditorState&, Document& doc) {
    if (doc.textBuffer_->size() == 1 && doc.textBuffer_->rowLength(0) == 0) {
        return;
    }

    doc.textBuffer_->deleteLine(doc.cursor_.getY());
    doc.cursor_.setY(std::min(doc.textBuffer_->size() - 1, doc.cursor_.getY()));
    doc.cursor_.setX(std::min(doc.textBuffer_->rowLength(doc.cursor_.getY()) - 1, doc.cursor_.getX()));

    if (doc.textView_.startY_ + doc.textView_.visibleLines_ >= doc.textBuffer_->size()) {
        doc.textView_.startY_ = std::min(0zu, doc.textView_.startY_ - 1);
    }
}

void NormalMode::deleteChar(EditorState&, Document& doc) {
	doc.textBuffer_->deleteCharacter(doc.cursor_.getY(), doc.cursor_.getX());
}

void NormalMode::findFirstCharRight(EditorState& editorState, Document& doc) {
    char symb = editorState.input_.back();
    const auto index = doc.textBuffer_->firstCharOccurrenceRight(doc.cursor_.getY(), doc.cursor_.getX(), symb);
    if (index.has_value()) {
        doc.cursor_.setX(index.value());
    }
}

void NormalMode::findFirstCharLeft(EditorState& editorState, Document& doc) {
    char symb = editorState.input_.back();
    const auto index = doc.textBuffer_->firstCharOccurrenceLeft(doc.cursor_.getY(), doc.cursor_.getX(), symb);
    if (index.has_value()) {
        doc.cursor_.setX(index.value());
    }
}

void NormalMode::insertLineAbove(EditorState& editorState, Document& doc) {
	doc.textBuffer_->insertLine(doc.cursor_.getY());
	doc.cursor_.setX(0);
	editorState.currentMode_ = Modes::Insert;

    if (doc.textView_.startY_ > doc.cursor_.getY()) {
        doc.textView_.startY_--;
    }
}
void NormalMode::insertLineBelow(EditorState& editorState, Document& doc) {
	doc.textBuffer_->insertLine(doc.cursor_.getY() + 1);
	doc.cursor_.incrementY();
	doc.cursor_.setX(0);
	editorState.currentMode_ = Modes::Insert;

    if (doc.textView_.startY_ + doc.textView_.visibleLines_ <= doc.cursor_.getY()) {
        doc.textView_.startY_++;
    }
}

void NormalMode::switchToInsertLeft(EditorState& editorState, Document&) {
	editorState.currentMode_ = Modes::Insert;
}
void NormalMode::switchToInsertRight(EditorState& editorState, Document& doc) {
	editorState.currentMode_ = Modes::Insert;
	doc.cursor_.incrementX();
}

void NormalMode::HandleKeyboardInput(EditorState& editorState, Document& document) {

	if (paramFunc_ != nullptr && paramCount_ + 1 == editorState.input_.size()) {
		paramFunc_(editorState, document);
		editorState.input_.clear();
		paramFunc_ = nullptr;
		paramCount_ = 0;

	} else if (const auto it = paramCommands_.find(editorState.input_); it != paramCommands_.end()) {
		paramFunc_ = it->second;
		paramCount_ = editorState.input_.size();

	} else if (const auto it = fixedCommands_.find(editorState.input_); it != fixedCommands_.end()) {
		it->second(editorState, document);
		editorState.input_.clear();
		paramFunc_ = nullptr;

	} else {
		const bool flag1 = std::ranges::any_of(paramCommands_, [&](const auto& c) {return c.first.starts_with(editorState.input_);});
		const bool flag2 = std::ranges::any_of(fixedCommands_, [&](const auto& c) {return c.first.starts_with(editorState.input_);});

		if (!flag1 && !flag2) {
			editorState.input_.clear();
			paramFunc_ = nullptr;
		}
	}
}