#pragma once

#include <string_view>

using namespace std::string_view_literals;

class EditorState;
class Document;

class InsertMode final {
public:
	InsertMode() = default;

	void HandleKeyboardInput(EditorState&, Document&) const;
};