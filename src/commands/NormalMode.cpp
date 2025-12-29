#include "commands/NormalMode.hpp"

#include "buffer/Files.hpp"
#include "core/Editor.hpp"

#include <algorithm>
#include <iostream>

NormalMode::NormalMode(): paramFunc{nullptr}, paramCount_{} {
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
           {"dd", [this](EditorState& state, Document& doc) { deleteLine(state, doc); }},
    };
}

void NormalMode::moveCursorLeft(EditorState&, Document& doc) {
    if (doc.cursor_.x_ > 0) {
        doc.cursor_.x_--;
    }
}

void NormalMode::moveCursorRight(EditorState&, Document& doc) {
    if (doc.textBuffer_->rowView(doc.cursor_.y_).length() - 1 > doc.cursor_.x_) {
        doc.cursor_.x_++;
    }
}

void NormalMode::moveCursorUp(EditorState&, Document& doc) {
    if (doc.cursor_.y_ > 0) {
        const size_t currRowLength = doc.textBuffer_->rowView(doc.cursor_.y_).length();
        doc.cursor_.y_--;
        const size_t nextRowLength = doc.textBuffer_->rowView(doc.cursor_.y_).length();

        if (currRowLength - 1 == doc.cursor_.x_ || nextRowLength - 1 <= doc.cursor_.x_) {
            doc.cursor_.x_ = nextRowLength - 1;
        }
    }
}

void NormalMode::moveCursorDown(EditorState&, Document& doc) {
    if (doc.textBuffer_->size() - 1 > doc.cursor_.y_) {
        const size_t currRowLength = doc.textBuffer_->rowView(doc.cursor_.y_).length();
        doc.cursor_.y_++;
        const size_t nextRowLength = doc.textBuffer_->rowView(doc.cursor_.y_).length();

        if (currRowLength - 1 == doc.cursor_.x_ || nextRowLength - 1 <= doc.cursor_.x_) {
            doc.cursor_.x_ = nextRowLength - 1;
        }
    }
}

void NormalMode::moveCursorTopFile(EditorState&, Document& doc) {
    doc.cursor_.y_ = 0;
    doc.cursor_.x_ = std::min(doc.textBuffer_->rowLength(doc.cursor_.y_) - 1, doc.cursor_.x_);
}

void NormalMode::moveCursorBottomFile(EditorState&, Document& doc) {
    doc.cursor_.y_ = doc.textBuffer_->size() - 1;
    doc.cursor_.x_ = std::min(doc.textBuffer_->rowLength(doc.cursor_.y_) - 1, doc.cursor_.x_);
}

void NormalMode::moveRightMost(EditorState&, Document& doc) {
    doc.cursor_.x_ = doc.textBuffer_->rowView(doc.cursor_.y_).length() - 1;
}

void NormalMode::moveLeftMost(EditorState&, Document& doc) {
    doc.cursor_.x_ = 0;
}

void NormalMode::deleteLine(EditorState&, Document& doc) {
    doc.textBuffer_->erase(doc.cursor_.y_);
    doc.cursor_.y_ = std::min(doc.textBuffer_->size() - 1, doc.cursor_.y_);
    doc.cursor_.x_ = std::min(doc.textBuffer_->rowLength(doc.cursor_.y_) - 1, doc.cursor_.x_);
}

void NormalMode::findFirstCharRight(EditorState& editorState, Document& doc) {
    char symb = editorState.input_.back();
    const auto index = doc.textBuffer_->firstCharOccurrenceRight(doc.cursor_.y_, doc.cursor_.x_, symb);
    if (index.has_value()) {
        doc.cursor_.x_ = index.value();
    }
}

void NormalMode::findFirstCharLeft(EditorState& editorState, Document& doc) {
    char symb = editorState.input_.back();
    const auto index = doc.textBuffer_->firstCharOccurrenceLeft(doc.cursor_.y_, doc.cursor_.x_, symb);
    if (index.has_value()) {
        doc.cursor_.x_ = index.value();
    }
}

void NormalMode::HandleKeyboardInput(EditorState& editorState, Document& document) {

	if (paramFunc != nullptr && paramCount_ + 1 == editorState.input_.size()) {
		paramFunc(editorState, document);
		editorState.input_.clear();
		paramFunc = nullptr;
		paramCount_ = 0;
		return;
	}

	if (const auto it = paramCommands_.find(editorState.input_); it != paramCommands_.end()) {
		paramFunc = it->second;
		paramCount_ = editorState.input_.size();
		return;
	}

	if (const auto it = fixedCommands_.find(editorState.input_); it != fixedCommands_.end()) {
		it->second(editorState, document);
		editorState.input_.clear();
		return;
	}

	const bool flag1 = std::ranges::any_of(paramCommands_, [&](const auto& c) {return c.first.starts_with(editorState.input_);});
	const bool flag2 = std::ranges::any_of(fixedCommands_, [&](const auto& c) {return c.first.starts_with(editorState.input_);});

	if (!flag1 && !flag2) {
		editorState.input_.clear();
	}
}