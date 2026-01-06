#include "commands/InsertMode.hpp"

#include "core/Editor.hpp"

void InsertMode::HandleKeyboardInput(EditorState& editorState, Document& document) const {
	const auto& textBuffer = document.textBuffer_;
	auto& cursor = document.cursor_;

	if (!editorState.input_.empty()) {
		textBuffer->insertCharacter(cursor.getY(), cursor.getX(), editorState.input_.at(0));
		cursor.incrementX();
		editorState.input_.clear();
	}
}