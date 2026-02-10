#include "Editor.hpp"

#include <SDL.h>
#include <iostream>

EditorState::EditorState(const FileId activeFileId_t) : currentMode_{Modes::Normal}, activeFileId_{activeFileId_t}, running_{true} {}

Editor::Editor(const int argc, char** argv) : files_{fileHandler_, argc, argv}, panes_{}, editorState_{0} {
}

std::string Editor::EncodeInput(const SDL_Event& event) {
	if (event.type == SDL_QUIT) {
		editorState_.running_ = false;
		return {};
	}
	auto& doc = files_.getFile(editorState_.activeFileId_).value().get();
	if (event.type == SDL_KEYDOWN) {
		switch (event.key.keysym.sym) {
		case SDLK_ESCAPE:
			editorState_.currentMode_ = Modes::Normal;
			editorInputAndOutput_.input_.clear();
			doc.cursor_.setX(
			    std::min(doc.cursor_.getX(), doc.textBuffer_->getLineLength(doc.cursor_.getY()) - 1));
			break;
		case SDLK_BACKSPACE:
			return std::string(1, static_cast<char>(SpecialKeys::Backspace));
		case SDLK_RETURN:
			return std::string(1, static_cast<char>(SpecialKeys::Enter));
		default:
			break;
		}
	} else if (event.type == SDL_TEXTINPUT) {
		if (event.text.text[0] == ':') {
			editorState_.currentMode_ = Modes::Command;
			editorInputAndOutput_.input_.clear();
			return ":";
		}
		return event.text.text;
	}
	return {};
}

void Editor::HandleKeyboardInput() {
	SDL_Event event;

	while (SDL_PollEvent(&event)) {

		std::string input = EncodeInput(event);

		if (input.empty())
			return;

		editorInputAndOutput_.input_.append(input);
		std::cout << "Input state: " << editorInputAndOutput_.input_ << '\n';

	    const auto file = std::move(files_.getFile(this->editorState_.activeFileId_));

		switch (editorState_.currentMode_) {
		case Modes::Normal:
			normalMode_.HandleKeyboardInput(editorState_, file);
			break;
		case Modes::Insert:
			insertMode_.HandleKeyboardInput(editorState_, file);
			break;
		case Modes::Command:
			commandMode_.HandleKeyboardInput(editorState_, fileHandler_, files_);
			break;
		}
	}
}