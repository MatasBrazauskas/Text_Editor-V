#pragma once

#include "commands/IMode.hpp"
#include "buffer/ITextBuffer.hpp"
#include "buffer/Cursor.hpp"
#include <unordered_map>
#include <functional>

using namespace std::string_view_literals;

using FixedFunc = std::function<void(ITextBuffer&, Cursor&)>;
using ParamFunc = std::function<void(ITextBuffer&, Cursor&, char)>;

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

	std::unordered_map<std::string, FixedFunc> fixedCommands_;
	std::unordered_map<std::string, ParamFunc> paramCommands_;

	bool isParam;
	ParamFunc paramFunc;
};