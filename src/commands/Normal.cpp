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
        } else if (nextRowLength -1 <= c.x_){
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
        } else if (nextRowLength -1 <= c.x_){
            c.x_ = nextRowLength - 1;
        }
    }
}

NormalMode::NormalMode() {
    commands_ = {
        { "h", [this](ITextBuffer& tb, Cursor& c) { moveCursorLeft(tb, c); } },
        { "j", [this](ITextBuffer& tb, Cursor& c) { moveCursorDown(tb, c); } },
        { "k", [this](ITextBuffer& tb, Cursor& c) { moveCursorUp(tb, c); } },
        { "l", [this](ITextBuffer& tb, Cursor& c) { moveCursorRight(tb, c); } },
    };
}

std::string_view NormalMode::name() const noexcept {
    return "Normal"sv;
}

void NormalMode::HandleKeyboardInput(std::string& input, ITextBuffer& textBuffer, Cursor& cursor) {
    std::cout << input << '\n';
    bool flag = std::ranges::any_of(commands_, [input](const auto& c) {return c.first.starts_with(input);});

    if (!flag) {
        input.clear();
        return;
    }

    if (auto it = commands_.find(input); it != commands_.end()) {
        it->second(textBuffer, cursor);
        input.clear();
    }

    std::cout << "Cursor: " <<  cursor.x_ << ' ' << cursor.y_ << '\n';
}

