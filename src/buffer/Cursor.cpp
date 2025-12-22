#include "Cursor.hpp"

Cursor::Cursor(const ITextBuffer& textBuffer): state_(false), x_{}, y_{}, textBuffer_{textBuffer} {}