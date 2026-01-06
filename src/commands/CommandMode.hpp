#pragma once

class EditorState;
class Document;

class CommandMode final {
public:
	CommandMode() = default;
	void HandleKeyboardInput(EditorState&, Document&) const;
};