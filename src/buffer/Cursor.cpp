#include "Cursor.hpp"

Cursor::Cursor(const ITextBuffer& textBuffer): visible_(false), x_{}, y_{}, textBuffer_{textBuffer} {}