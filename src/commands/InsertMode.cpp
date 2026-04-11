#include "commands/InsertMode.hpp"

#include "core/EditorCore.hpp"

#include <iostream>
#include <utility>

InsertMode::InsertMode() {
	commands_ = {{static_cast<char>(SpecialKeys::Enter), &InsertMode::handleEnter},
				 {static_cast<char>(SpecialKeys::Backspace), &InsertMode::handleBackspace},
				 {static_cast<char>(SpecialKeys::Tab), &InsertMode::handleTab}};
}

void InsertMode::handleEnter(File& t_file, Cursor& t_cursor, const Config&) const {
	auto& [text, stack, path, fileId] = t_file;
	const auto subRange = text.getLineSubstr(t_cursor.getY(), t_cursor.getX());

	text.insertLine(t_cursor.getY() + 1, "");
	text.insertRange(t_cursor.getY() + 1, 0, subRange);

	text.deleteRange(t_cursor.getY(), t_cursor.getX(), text.getLineLength(t_cursor.getY()) - t_cursor.getX());

	t_cursor.incrementY();
	t_cursor.setX(0);
}
void InsertMode::handleBackspace(File& t_file, Cursor& t_cursor, const Config&) const {
	auto& [text, stack, path, fileId] = t_file;

	if (t_cursor.getX() == 0 && t_cursor.getY() > 0) {
		const auto movedLine = text.getLine(t_cursor.getY());
		const size_t linesLength = movedLine.length();

		const size_t currLineIndex = t_cursor.getY();
		const size_t prevLineIndex = t_cursor.getY() - 1;

		text.insertRange(prevLineIndex, text.getLineLength(prevLineIndex), movedLine);
		text.deleteLine(currLineIndex);

		t_cursor.decrementY();
		t_cursor.setX(text.getLineLength(t_cursor.getY()) - linesLength);
	} else if (t_cursor.getX() > 0 && t_cursor.getY() >= 0) {
		text.deleteCharacter(t_cursor.getY(), t_cursor.getX() - 1);
		t_cursor.decrementX();
	}
}

void InsertMode::handleTab(File& t_file, Cursor& t_cursor, const Config& t_config) const {
	auto& [text, stack, path, fileId] = t_file;
	const auto indentType = t_config.editor.feel.indentType;
	const auto indentSize = t_config.editor.feel.indentSize;

	if (indentType == IndentType::Space) {
		const auto spaceIndent = std::string(indentSize, ' ');

		text.insertRange(t_cursor.getY(), t_cursor.getX(), std::string_view{spaceIndent});
		t_cursor.setX(t_cursor.getX() + indentSize);
	} else if (indentType == IndentType::Tabs) {
		throw std::runtime_error("Inplement");
	}
}

void InsertMode::HandleKeyboardInput(EditorInputAndOutput& t_io, File& t_file, Cursor& t_cursor, const Config& t_config) const {
	if (t_io.input_.empty())
		return;

	if (const auto it = commands_.find(t_io.input_.back()); it != commands_.end()) {
		const auto func = it->second;
		(this->*func)(t_file, t_cursor, t_config);
	} else {
		t_file.textBuffer_.insertCharacter(t_cursor.getY(), t_cursor.getX(), t_io.input_.back());
		t_cursor.incrementX();
	}

	t_io.cleanInputs();
}