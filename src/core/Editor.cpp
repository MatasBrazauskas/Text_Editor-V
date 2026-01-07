#include "Editor.hpp"

#include <iostream>
#include <SDL.h>

EditorState::EditorState(): activeTab_{}, currentMode_{Modes::Normal}, running_{true} {}

Editor::Editor(Files& files, EditorState& editorState): files_{files}, editorState_ {editorState} {}

void Editor::HandleKeyboardInput() {
	SDL_Event event;

	while (SDL_PollEvent(&event)) {

		if (event.type == SDL_QUIT) {
			editorState_.running_ = false;
			return;
		}
		if (event.type == SDL_KEYDOWN) {
			if (event.key.keysym.sym == SDLK_ESCAPE) {
				editorState_.currentMode_ = Modes::Normal;
				editorState_.input_.clear();
				std::cout << "Switched to normal mode\n";
				return;
			}
		} else if (event.type == SDL_TEXTINPUT) {
			editorState_.input_.append(event.text.text);
		}

		if (editorState_.input_.empty()) return;

		std::cout << "Input state: " << editorState_.input_ << '\n';

		switch (editorState_.currentMode_) {
			case Modes::Normal: normalMode_.HandleKeyboardInput(editorState_, files_.getDocument(editorState_.activeTab_)); break;
			case Modes::Insert: insertMode_.HandleKeyboardInput(editorState_, files_.getDocument(editorState_.activeTab_)); break;
			case Modes::Command: commandMode_.HandleKeyboardInput(editorState_, files_.getDocument(editorState_.activeTab_)); break;

		}
	}
}