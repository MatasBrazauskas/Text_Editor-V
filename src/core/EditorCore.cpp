#include "EditorCore.hpp"

#include <algorithm>
#include <SDL.h>
#include <iostream>

EditorState::EditorState(const FileId activeFileId_t)
    : currentMode_{Modes::Normal}, activeFileId_{activeFileId_t}, running_{true} {}

EditorCore::EditorCore(const int argc, char** argv) : filesManager_{fileHandler_, argc, argv}, panesManager_{}, editorState_{0} {}

std::string EditorCore::EncodeInput(const SDL_Event& event) {
	if (event.type == SDL_QUIT) {
		editorState_.running_ = false;
		return {};
	}
	auto& doc = filesManager_.getFile(editorState_.activeFileId_).get();
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

void EditorCore::HandleKeyboardInput() {
	SDL_Event event;
    dirty = false;

	while (SDL_PollEvent(&event)) {

		std::string input = EncodeInput(event);

		if (input.empty())
			return;

		editorInputAndOutput_.input_.append(input);
		std::cout << "Input state: " << editorInputAndOutput_.input_ << '\n';

		const auto file = filesManager_.getFile(this->editorState_.activeFileId_);

		switch (editorState_.currentMode_) {
		case Modes::Normal:
			normalMode_.HandleKeyboardInput(file, editorState_, editorInputAndOutput_);
			break;
		case Modes::Insert:
			insertMode_.HandleKeyboardInput(editorState_, editorInputAndOutput_, file);
			break;
		case Modes::Command:
			commandMode_.HandleKeyboardInput(editorState_, fileHandler_, filesManager_);
			break;
		}

	    dirty = true;
	}
}

const FilesManager& EditorCore::getFiles() const {
    return filesManager_;
}

const PanesManager& EditorCore::getPanesManager() const {
    return panesManager_;
}

const EditorState& EditorCore::getEditorState() const {
    return editorState_;
}

const EditorInputAndOutput& EditorCore::getEditorInputAndOutput() const {
    return editorInputAndOutput_;
}
