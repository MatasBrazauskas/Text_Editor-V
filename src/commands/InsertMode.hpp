#pragma once

#include <string>
#include <unordered_map>

class EditorState;
class Document;

class InsertMode final {
public:
	InsertMode();
	~InsertMode() = default;

	void HandleKeyboardInput(EditorState&, std::reference_wrapper<Document>) const;
	using Func = void (InsertMode::*)(EditorState&, Document&) const;

private:
	void handleEnter(EditorState&, Document&) const;
	void handleBackspace(EditorState&, Document&) const;

	std::unordered_map<std::string, Func> commands_;
};