#pragma once

class EditorState;
class Document;

class IMode {
public:
	virtual ~IMode() noexcept = default;

	virtual void HandleKeyboardInput(EditorState&, Document&) = 0;
};