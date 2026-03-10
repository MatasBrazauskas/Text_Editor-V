#include "EditorCore.hpp"

#include "utils/ConfigAndSettings.hpp"

#include <SDL.h>
#include <iostream>

EditorState::EditorState(const FileId activeFileId_t)
	: currentMode_{Modes::Normal}, activeFileId_{activeFileId_t}, running_{true} {}

EditorCore::EditorCore(const int argc, char** argv, Settings& t_settings)
	: dirty{true}, filesManager_{fileHandler_, argc, argv}, panesManager_{}, editorState_{0}, settings_{t_settings} {
	panesManager_.addPane(0, 0, AddedPaneRotation::Top);
}

std::string EditorCore::EncodeInput(const SDL_Event& event) {
	if (event.type == SDL_QUIT) {
		editorState_.running_ = false;
		return {};
	}
	auto& doc = filesManager_.getFile(editorState_.activeFileId_);
	if (event.type == SDL_KEYDOWN) {
		switch (event.key.keysym.sym) {
		case SDLK_ESCAPE:
			editorState_.currentMode_ = Modes::Normal;
			editorInputAndOutput_.input_.clear();
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

	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_WINDOWEVENT) {
			if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
				settings_.windowSettings.width = event.window.data1;
				settings_.windowSettings.height = event.window.data2;
			}
		}

		const std::string input = EncodeInput(event);

		if (input.empty())
			return;

		editorInputAndOutput_.input_.append(input);
		std::cout << "Input state: " << editorInputAndOutput_.input_ << '\n';

		auto file = filesManager_.getFile(this->editorState_.activeFileId_);

		const auto paneOpt = panesManager_.getCurrPane();

		if (paneOpt == std::nullopt) {
			throw std::runtime_error("No pane");
		}

		auto cursor = paneOpt.value().cursor_;

		switch (editorState_.currentMode_) {
		case Modes::Normal:
			normalMode_.HandleKeyboardInput(file, cursor, editorState_, editorInputAndOutput_);
			break;
		case Modes::Insert:
			insertMode_.HandleKeyboardInput(editorState_, editorInputAndOutput_, file, cursor);
			break;
		case Modes::Command:
			commandMode_.HandleKeyboardInput(editorState_, editorInputAndOutput_, fileHandler_, filesManager_);
			break;
		}

		dirty = true;
	}
}

FilesManager& EditorCore::getFilesManager() {
	return filesManager_;
}

PanesManager& EditorCore::getPanesManager() {
	return panesManager_;
}

const EditorState& EditorCore::getEditorState() const {
	return editorState_;
}

const EditorInputAndOutput& EditorCore::getEditorInputAndOutput() const {
	return editorInputAndOutput_;
}
