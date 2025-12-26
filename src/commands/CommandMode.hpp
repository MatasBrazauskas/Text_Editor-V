#pragma once

#include "IMode.hpp"
#include "buffer/ITextBuffer.hpp"
#include "buffer/Cursor.hpp"

#include <string>

class CommandMode final : public IMode {
public:
	std::string input_;

	[[nodiscard]] std::string_view name() const noexcept override;

	void HandleKeyboardInput(std::string& input, ITextBuffer&, Cursor&) override;
};