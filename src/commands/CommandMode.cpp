#include "CommandMode.hpp"

std::string_view CommandMode::name() const noexcept {
	return "param";
}

void CommandMode::HandleKeyboardInput(std::string& input, ITextBuffer& textBuffer, Cursor& cursor) {

}
