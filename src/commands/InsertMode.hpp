#pragma once

#include <unordered_map>
#include <string>

class EditorState;
class Document;

class InsertMode final {
public:
	InsertMode();
    ~InsertMode() = default;

	void HandleKeyboardInput(EditorState&, Document&) const;
    using Func = void (InsertMode::*)(EditorState&, Document&) const;

private:
    void handleEnter(EditorState&, Document&) const;
    void handleBackspace(EditorState&, Document&) const;

    std::unordered_map<std::string, Func> commands_;
};