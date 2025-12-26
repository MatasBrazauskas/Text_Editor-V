#include "Editor.hpp"

#include <iostream>
#include <SDL.h>

Editor::Editor(ITextBuffer& textBuffer, Cursor& cursor) : textBuffer_(textBuffer), cursor_(cursor),
                                                          running_(true) {
	normalMode_ = std::make_unique<NormalMode>();
	insertMode_ = std::make_unique<InsertMode>();

	mode_ = normalMode_.get();
}

void Editor::switchToInsertMode() {
	mode_ = insertMode_.get();
}

void Editor::switchToNormalMode() {
	mode_ = normalMode_.get();
}

void Editor::switchToCommandMode() {
	mode_ = commandMode_.get();
}

void Editor::HandleKeyboardInput() {
	SDL_Event event;

	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT) {
			running_ = false;
			std::cout << "Quit programme\n";
			return;
		}
		if (event.type == SDL_KEYDOWN) {
			if (event.key.keysym.sym == SDLK_ESCAPE) {
				switchToNormalMode();
				input_.clear();
				std::cout << "Switched to normal mode\n";
				return;
			}
		} else if (event.type == SDL_TEXTINPUT) {
			input_.append(event.text.text);
			if (input_ == "i") {
				switchToInsertMode();
				input_.clear();
				std::cout << "Switched to insert mode\n";
				return;
			} else if (input_ == ":") {
				switchToCommandMode();
				input_.clear();
				std::cout << "Switched to command mode\n";
				return;
			}
		}

		mode_->HandleKeyboardInput(input_, textBuffer_, cursor_);
	}
}