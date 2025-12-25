#pragma once
#include <string_view>
#include <SDL.h>
#include "buffer/ITextBuffer.hpp"
#include "buffer/Cursor.hpp"

class IMode {
public:
	virtual ~IMode() = default;

	[[nodiscard]] virtual std::string_view name() const noexcept = 0;

	virtual void HandleKeyboardInput(std::string& input, ITextBuffer&, Cursor&) = 0;
};