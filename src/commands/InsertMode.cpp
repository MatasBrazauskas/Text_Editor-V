#include "commands/InsertMode.hpp"

#include "buffer/Files.hpp"
#include "core/Editor.hpp"

void InsertMode::HandleKeyboardInput(EditorState& editorState, Document& document) {
	auto& textBuffer = document.textBuffer_;
	auto& cursor = document.cursor_;

	if (!editorState.input_.empty()) {
		auto& row = textBuffer->rowRef(cursor.y_);
		row.insert(cursor.x_++, editorState.input_);
		editorState.input_.clear();
	}
}