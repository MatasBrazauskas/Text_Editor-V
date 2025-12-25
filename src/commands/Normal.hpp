#pragma once

#include "commands/IMode.hpp"
#include "buffer/ITextBuffer.hpp"
#include "buffer/Cursor.hpp"
#include <unordered_map>
#include <functional>

using namespace std::string_view_literals;

class NormalMode final : public IMode {
public:
	NormalMode();

	[[nodiscard]] std::string_view name() const noexcept override;

	void HandleKeyboardInput(std::string&, ITextBuffer&, Cursor&) override;

	void moveCursorLeft(ITextBuffer&, Cursor&);

	void moveCursorDown(ITextBuffer&, Cursor&);

	void moveCursorUp(ITextBuffer&, Cursor&);

	void moveCursorRight(ITextBuffer&, Cursor&);

	void moveCursorTopFile(ITextBuffer&, Cursor&);

	void moveCursorBottomFile(ITextBuffer&, Cursor&);

	void moveRightMost(ITextBuffer&, Cursor&);

	void moveLeftMost(ITextBuffer&, Cursor&);

	void deleteLine(ITextBuffer&, Cursor&);

	void findFirstCharRight(ITextBuffer&, Cursor&, char);
	void findFirstCharLeft(ITextBuffer&, Cursor&, char);

	std::unordered_map<std::string, std::function<void(ITextBuffer&, Cursor&)> > fixedCommands_;
	std::unordered_map<std::string, std::function<void(ITextBuffer&, Cursor&, char)>> paramCommands_;
};