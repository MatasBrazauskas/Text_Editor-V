#pragma once

#include "commands/IMode.hpp"
#include <SDL.h>
#include "buffer/ITextBuffer.hpp"
#include "buffer/Cursor.hpp"

using namespace std::string_view_literals;

class InsertMode: public IMode {
public:
    InsertMode() = default;
    [[nodiscard]] std::string_view name() const noexcept override;

    void HandleKeyboardInput(std::string& input, ITextBuffer& textBuffer, Cursor& cursor) override;
};