#include "commands/InsertMode.hpp"

#include <core/Editor.hpp>

constexpr char enterKey = static_cast<char>(SpecialKeys::Enter);
constexpr char backSpaceKey = static_cast<char>(SpecialKeys::Backspace);

InsertMode::InsertMode() {
	commands_ = {{enterKey, &InsertMode::handleEnter}, {backSpaceKey, &InsertMode::handleBackspace}};
}

void InsertMode::handleEnter(EditorState&, Document& doc) const {
	const auto subRange = doc.textBuffer_->rowSubstr(doc.cursor_.getY(), doc.cursor_.getX());

	doc.textBuffer_->insertLine(doc.cursor_.getY() + 1, std::string{subRange});
	// doc.textBuffer_->insertRange(doc.cursor_.getY() + 1, 0, subRange);

	doc.textBuffer_->deleteRange(doc.cursor_.getY(), doc.cursor_.getX(),
				     doc.textBuffer_->rowsLength(doc.cursor_.getY()) - doc.cursor_.getX());

	doc.cursor_.incrementY();
	doc.cursor_.setX(0);
}
void InsertMode::handleBackspace(EditorState&, Document& doc) const {
	if (doc.cursor_.getX() == 0 && doc.cursor_.getY() > 0) {
		const auto movedLine = doc.textBuffer_->rowsView(doc.cursor_.getY());
		const size_t linesLength = movedLine.length();

		doc.textBuffer_->insertRange(doc.cursor_.getY() - 1, doc.textBuffer_->rowsLength(doc.cursor_.getY() - 1),
					     movedLine);
		doc.textBuffer_->deleteLine(doc.cursor_.getY());

		doc.cursor_.decrementY();
		doc.cursor_.setX(doc.textBuffer_->rowsLength(doc.cursor_.getY()) - linesLength);
	} else if (doc.cursor_.getX() > 0 && doc.cursor_.getY() >= 0) {
		doc.textBuffer_->deleteCharacter(doc.cursor_.getY(), doc.cursor_.getX() - 1);
		doc.cursor_.decrementX();
	}
}

void InsertMode::HandleKeyboardInput(EditorState& editorState, std::reference_wrapper<Document> doc) const {
	if (editorState.input_.empty())
		return;

	auto& [text, _, cursor, idk] = doc.get();

	if (const auto it = commands_.find(editorState.input_.back()); it != commands_.end()) {
		const auto func = it->second;
		(this->*func)(editorState, doc);
	} else {
		text->insertCharacter(cursor.getY(), cursor.getX(), editorState.input_.at(0));
		cursor.incrementX();
	}

	editorState.input_.clear();
}