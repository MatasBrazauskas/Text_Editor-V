#include "commands/InsertMode.hpp"

#include <core/EditorCore.hpp>

constexpr char enterKey = static_cast<char>(SpecialKeys::Enter);
constexpr char backSpaceKey = static_cast<char>(SpecialKeys::Backspace);

InsertMode::InsertMode() {
	commands_ = {{enterKey, &InsertMode::handleEnter}, {backSpaceKey, &InsertMode::handleBackspace}};
}

void InsertMode::handleEnter(File& file) const {
    auto& [text, _, cursor, idk, f] = file;
	const auto subRange = text->getLineSubstr(cursor.getY(), cursor.getX());

	text->insertLine(cursor.getY() + 1, std::string{subRange});
	text->deleteRange(cursor.getY(), cursor.getX(),text->getLineLength(cursor.getY()) - cursor.getX());

	cursor.incrementY();
	cursor.setX(0);
}
void InsertMode::handleBackspace(File& file_t) const {
    auto& [text, _, cursor, idk, f] = file_t;

	if (cursor.getX() == 0 && cursor.getY() > 0) {
		const auto movedLine = text->getLine(cursor.getY());
		const size_t linesLength = movedLine.length();

		text->insertRange(cursor.getY() - 1, text->getLineLength(cursor.getY() - 1), movedLine);
		text->deleteLine(cursor.getY());

		cursor.decrementY();
		cursor.setX(text->getLineLength(cursor.getY()) - linesLength);
	} else if (cursor.getX() > 0 && cursor.getY() >= 0) {
		text->deleteCharacter(cursor.getY(), cursor.getX() - 1);
		cursor.decrementX();
	}
}

void InsertMode::HandleKeyboardInput(EditorState& state_t, EditorInputAndOutput& inputOutput_t, std::reference_wrapper<File> file_t) const {
	if (inputOutput_t.input_.empty())
		return;

	auto& file = file_t.get();
    auto& [text, _, cursor, idk, f] = file;

	if (const auto it = commands_.find(inputOutput_t.input_.back()); it != commands_.end()) {
		const auto func = it->second;
		(this->*func)(file);
	} else {
		text->insertCharacter(cursor.getY(), cursor.getX(), inputOutput_t.input_.at(0));
		cursor.incrementX();
	}

	inputOutput_t.input_.clear();
}