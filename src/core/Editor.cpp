#include "Editor.hpp"

#include <iostream>
#include <SDL.h>

EditorState::EditorState(): activeTab_{}, currentMode_{Modes::Normal}, running_{true} {}

Editor::Editor(Files& files, FileHandler& fileHandler, EditorState& editorState): files_{files}, fileHandler_{fileHandler}, editorState_ {editorState} {}

std::string Editor::EncodeInput(const SDL_Event& event) {
    if (event.type == SDL_QUIT) {
        editorState_.running_ = false;
        return {};
    }
    if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
            case SDLK_ESCAPE:
                editorState_.currentMode_ = Modes::Normal;
                editorState_.input_.clear();
                break;
            case SDLK_BACKSPACE: return std::string(1, static_cast<char>(SpecialKeys::Backspace));
            case SDLK_RETURN: return std::string(1, static_cast<char>(SpecialKeys::Enter));
            default:
                std::cout << "Bybys zino\n";
        }
    } else if (event.type == SDL_TEXTINPUT) {
        if (event.text.text[0] == ':') {
            editorState_.currentMode_ = Modes::Command;
            editorState_.input_.clear();
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

		if (input.empty()) return;

	    editorState_.input_.append(input);
		std::cout << "Input state: " << editorState_.input_ << '\n';

		switch (editorState_.currentMode_) {
			case Modes::Normal: normalMode_.HandleKeyboardInput(editorState_, files_.getDocument(editorState_.activeTab_)); break;
			case Modes::Insert: insertMode_.HandleKeyboardInput(editorState_, files_.getDocument(editorState_.activeTab_)); break;
			case Modes::Command: commandMode_.HandleKeyboardInput(editorState_, fileHandler_, files_); break;

		}
	}
}