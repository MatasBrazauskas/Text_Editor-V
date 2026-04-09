#include "commands/InsertMode.hpp"

#include "core/EditorCore.hpp"

#include <utility>

InsertMode::InsertMode() {
	commands_ = {{static_cast<char>(SpecialKeys::Enter), &InsertMode::handleEnter},
				 {static_cast<char>(SpecialKeys::Backspace), &InsertMode::handleBackspace}};
}

void InsertMode::handleEnter(File& t_file, Cursor& t_cursor) const {
	auto& [text, stack, path, fileId] = t_file;
	const auto subRange = text.getLineSubstr(t_cursor.getY(), t_cursor.getX());

	text.insertLine(t_cursor.getY() + 1, "");
	text.insertRange(t_cursor.getY() + 1, 0, subRange);

	text.deleteRange(t_cursor.getY(), t_cursor.getX(), text.getLineLength(t_cursor.getY()) - t_cursor.getX());

	t_cursor.incrementY();
	t_cursor.setX(0);
}
void InsertMode::handleBackspace(File& t_file, Cursor& t_cursor) const {
	auto& [text, stack, path, fileId] = t_file;

	if (t_cursor.getX() == 0 && t_cursor.getY() > 0) {
		const auto movedLine = text.getLineSubstr(t_cursor.getY(), 0);
		const size_t linesLength = movedLine.length();

		text.insertRange(t_cursor.getY() - 1, text.getLineLength(t_cursor.getY() - 1), movedLine);
		text.deleteLine(t_cursor.getY() - 1);

		t_cursor.decrementY();
		t_cursor.setX(text.getLineLength(t_cursor.getY()) - linesLength);
	} else if (t_cursor.getX() > 0 && t_cursor.getY() >= 0) {
		text.deleteCharacter(t_cursor.getY(), t_cursor.getX() - 1);
		t_cursor.decrementX();
	}
}

void InsertMode::HandleKeyboardInput(EditorState& t_state, EditorInputAndOutput& t_io, File& t_file,
									 Cursor& t_cursor) const {
	if (t_io.input_.empty())
		return;

	if (const auto it = commands_.find(t_io.input_.back()); it != commands_.end()) {
		const auto func = it->second;
		(this->*func)(t_file, t_cursor);
	} else {
		t_file.textBuffer_.insertCharacter(t_cursor.getY(), t_cursor.getX(), t_io.input_.back());
		t_cursor.incrementX();
	}

	t_io.input_.clear();
}