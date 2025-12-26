#pragma once
#include "buffer/Cursor.hpp"
#include "buffer/ITextBuffer.hpp"
#include "commands/CommandMode.hpp"
#include "commands/Insert.hpp"
#include "commands/Normal.hpp"
#include <memory>

class Editor final {
public:
	Editor();

	Editor(ITextBuffer&, Cursor&);

	~Editor() = default;

	void HandleKeyboardInput();

	void switchToInsertMode();

	void switchToNormalMode();

	void switchToCommandMode();

	ITextBuffer& textBuffer_;
	Cursor&      cursor_;

	bool        running_;
	std::string input_;

	std::unique_ptr<NormalMode> normalMode_;
	std::unique_ptr<InsertMode> insertMode_;
	std::unique_ptr<CommandMode> commandMode_;
	IMode*                      mode_;
};