#pragma once

#include <string_view>

#include "commands/IMode.hpp"

using namespace std::string_view_literals;

class InsertMode final : IMode {
public:
	InsertMode() = default;

	void HandleKeyboardInput(EditorState&, Document&) override;
};