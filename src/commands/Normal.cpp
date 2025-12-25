#include "commands/Normal.hpp"

#include <algorithm>
#include <iostream>
#include <ostream>
#include <cmath>

using namespace std::string_view_literals;

void NormalMode::moveCursorLeft(ITextBuffer& tb, Cursor& c) {
	if (c.x_ > 0) {
		c.x_--;
	}
}

void NormalMode::moveCursorRight(ITextBuffer& tb, Cursor& c) {
	if (tb.rowView(c.y_).length() - 1 > c.x_) {
		c.x_++;
	}
}

// for improvement - keep track of a cursor position and just moving - like in normal move cursor saves the position of prev accurance.
void NormalMode::moveCursorUp(ITextBuffer& tb, Cursor& c) {
	if (c.y_ > 0) {
		const size_t currRowLength = tb.rowView(c.y_).length();
		c.y_--;
		const size_t nextRowLength = tb.rowView(c.y_).length();

		if (currRowLength - 1 == c.x_) {
			c.x_ = nextRowLength - 1;
		} else if (nextRowLength - 1 <= c.x_) {
			c.x_ = nextRowLength - 1;
		}
	}
}

void NormalMode::moveCursorDown(ITextBuffer& tb, Cursor& c) {
	if (tb.size() - 1 > c.y_) {
		const size_t currRowLength = tb.rowView(c.y_).length();
		c.y_++;
		const size_t nextRowLength = tb.rowView(c.y_).length();

		if (currRowLength - 1 == c.x_) {
			c.x_ = nextRowLength - 1;
		} else if (nextRowLength - 1 <= c.x_) {
			c.x_ = nextRowLength - 1;
		}
	}
}

void NormalMode::moveCursorTopFile(ITextBuffer& tb, Cursor& c) {
	c.y_ = 0;
	c.x_ = std::min(tb.rowLength(c.y_) - 1, c.x_);
}

void NormalMode::moveCursorBottomFile(ITextBuffer& tb, Cursor& c) {
	c.y_ = tb.size() - 1;
	c.x_ = std::min(tb.rowLength(c.y_) - 1, c.x_);
}

void NormalMode::moveRightMost(ITextBuffer& tb, Cursor& c) {
	c.x_ = tb.rowView(c.y_).length() - 1;
}

void NormalMode::moveLeftMost(ITextBuffer&, Cursor& c) {
	c.x_ = 0;
}

void NormalMode::deleteLine(ITextBuffer& tb, Cursor& c) {
	tb.erase(c.y_);

	c.y_ = std::min(tb.size() - 1, c.y_);
	c.x_ = std::min(tb.rowLength(c.y_) - 1, c.x_);
}

void NormalMode::findFirstCharRight(ITextBuffer& tb, Cursor& c, char symb) {
	const auto index = tb.firstCharAccuranceRight(c.y_, c.x_, symb);

	if (index.has_value()) {
		c.x_ = index.value();
	}
}
void NormalMode::findFirstCharLeft(ITextBuffer& tb, Cursor& c, char symb) {
	const auto index = tb.firstCharAccuranceLeft(c.y_, c.x_, symb);

	if (index.has_value()) {
		c.x_ = index.value();
	}
}

NormalMode::NormalMode(): isParam{false}, paramFunc{nullptr} {
	paramCommands_ = {
		{"f", [this](ITextBuffer& tb, Cursor& c, char symb) {return findFirstCharRight(tb, c, symb);}},
		{"F", [this](ITextBuffer& tb, Cursor& c, char symb) {return findFirstCharLeft(tb, c, symb);}},
	};

	fixedCommands_ = {
		{"h", [this](ITextBuffer& tb, Cursor& c) {moveCursorLeft(tb, c);}},
		{"j", [this](ITextBuffer& tb, Cursor& c) {moveCursorDown(tb, c);}},
		{"k", [this](ITextBuffer& tb, Cursor& c) {moveCursorUp(tb, c);}},
		{"l", [this](ITextBuffer& tb, Cursor& c) {moveCursorRight(tb, c);}},
		{"gg", [this](ITextBuffer& tb, Cursor& c) {moveCursorTopFile(tb, c);}},
		{"G", [this](ITextBuffer& tb, Cursor& c) {moveCursorBottomFile(tb, c);}},
		{"$", [this](ITextBuffer& tb, Cursor& c) {moveRightMost(tb, c);}},
		{"0", [this](ITextBuffer& tb, Cursor& c) {moveLeftMost(tb, c);}},
		{"dd", [this](ITextBuffer& tb, Cursor& c) {deleteLine(tb, c);}},
	};
}

std::string_view NormalMode::name() const noexcept {
	return "Normal"sv;
}

void NormalMode::HandleKeyboardInput(std::string& input, ITextBuffer& textBuffer, Cursor& cursor) {

	std::cout << '|' << input << "|\n";

	if (isParam && !input.empty()) {
		paramFunc(textBuffer, cursor, input.back());
		paramFunc = nullptr;
		isParam = false;
		return;
	}

	if (const auto it = paramCommands_.find(input); it != paramCommands_.end()) {
		paramFunc = it->second;
		isParam = true;
		input.clear();
		return;
	}

	if (const auto it = fixedCommands_.find(input); it != fixedCommands_.end()) {
		it->second(textBuffer, cursor);
		input.clear();
		return;
	}

	bool flag1 = std::ranges::any_of(paramCommands_, [input](const auto& c) {return c.first.starts_with(input);});
	bool flag2 = std::ranges::any_of(fixedCommands_, [input](const auto& c) {return c.first.starts_with(input);});

	if (!flag1 && !flag2) {
		input.clear();
		return;
	}

	std::cout << "Cursor: " << cursor.x_ << ' ' << cursor.y_ << '\n';
}