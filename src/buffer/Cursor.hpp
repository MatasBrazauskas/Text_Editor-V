#pragma once
#include <cstddef>
#include "buffer/ITextBuffer.hpp"

class Cursor {
public:
    Cursor() = delete;
    Cursor(const ITextBuffer& textBuffer);

    bool visible_;
    std::size_t x_;
    std::size_t y_;
    const ITextBuffer& textBuffer_;
};