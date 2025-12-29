#pragma once

#include "commands/IMode.hpp"

class CommandMode final : public IMode {
public:
	CommandMode() = default;
	void HandleKeyboardInput(EditorState&, Document&) override;
};