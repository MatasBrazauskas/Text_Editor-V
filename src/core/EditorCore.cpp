#include "EditorCore.hpp"

#include "utils/ConfigAndSettings.hpp"

#include <SDL.h>

EditorState::EditorState() : currentMode_{Modes::Normal}, running_{true} {}

EditorCore::EditorCore(const int argc, char** argv, Settings& t_settings): filesManager_{fileHandler_, argc, argv}, settings_{t_settings} {
	panesManager_.addPane(0, 0, PaneDirection::Top);
}

std::variant<SpecialCases, std::string> EditorCore::EncodeInput(const SDL_Event& t_event) {
	if (t_event.type == SDL_QUIT) {
		return SpecialCases::Quit;
	}

	if (t_event.type == SDL_KEYDOWN) {
		const SDL_Keycode keyCode = t_event.key.keysym.sym;
		if (keyCode == SDLK_ESCAPE) {
			return SpecialCases::SwitchToNormalMode;
		}

		if (keyCode == SDLK_LCTRL || keyCode == SDLK_RCTRL) {
			return std::string(1, static_cast<char>(SpecialKeys::Shift));
		}

		if (const auto it = specialKeyMap.find(keyCode); it != specialKeyMap.end()) {
			return std::string(1, it->second);
		}

	} else if (t_event.type == SDL_TEXTINPUT) {
		return t_event.text.text;
	} else if (t_event.type == SDL_WINDOWEVENT) {
		if (t_event.window.event == SDL_WINDOWEVENT_RESIZED) {
			return SpecialCases::WindowResize;
		}
	}
	return SpecialCases::None;
}

;
void EditorCore::HandleSpecialCases(const SpecialCases t_specialCase, const SDL_Event& t_event) {
	const auto cleanUp = [this] {
		editorInputAndOutput_.input_.clear();
		editorInputAndOutput_.commandLineMessage_.clear();
	};

	switch (t_specialCase) {
	case SpecialCases::SwitchToNormalMode:
		editorState_.currentMode_ = Modes::Normal;
		cleanUp();
		break;
	case SpecialCases::SwitchToInsertMode:
		editorState_.currentMode_ = Modes::Insert;
		cleanUp();
		break;
	case SpecialCases::SwitchToCommandMode:
		editorState_.currentMode_ = Modes::Command;
		cleanUp();
		break;
	case SpecialCases::SwitchToFileMode:
		editorState_.currentMode_ = Modes::FileMode;
		cleanUp();
		break;
	case SpecialCases::SwitchToWindowMode:
		editorState_.currentMode_ = Modes::WindowMode;
		cleanUp();
		break;
	case SpecialCases::WindowResize:
		settings_.windowSettings.width = t_event.window.data1;
		settings_.windowSettings.height = t_event.window.data2;
		break;
	case SpecialCases::Quit:
		editorState_.running_ = false;
		break;
	case SpecialCases::None:
		break;
	}
}

void EditorCore::HandleKeyboardInput() {
	SDL_Event event;
	auto winSettings = settings_.windowSettings;

	while (SDL_PollEvent(&event)) {
		const auto input = EncodeInput(event);

		if (std::holds_alternative<SpecialCases>(input)) {
			const auto specialCase = std::get<SpecialCases>(input);
			HandleSpecialCases(specialCase, event);
		} else {
			editorInputAndOutput_.input_.append(std::get<std::string>(input));
			editorInputAndOutput_.commandLineMessage_.append(std::get<std::string>(input));

			auto& file = filesManager_.getFile();
			auto& cursor = panesManager_.getCurrPane()->get().cursor_;

			switch (editorState_.currentMode_) {
			case Modes::Normal:
				normalMode_.HandleKeyboardInput(file, cursor, editorState_, editorInputAndOutput_);
				break;
			case Modes::Insert:
				insertMode_.HandleKeyboardInput(editorState_, editorInputAndOutput_, file, cursor);
				break;
			case Modes::Command:
				commandMode_.HandleKeyboardInput(editorState_, editorInputAndOutput_, fileHandler_, filesManager_,
												 panesManager_);
				break;
			case Modes::WindowMode:
				windowSubCommand_.ExecuteCommand(panesManager_, winSettings, editorInputAndOutput_.input_.back());
				break;
			case Modes::FileMode:
				fileSubCommand_.ExecuteCommand(panesManager_, filesManager_, winSettings,
											   editorInputAndOutput_.input_.back());
				break;
			}
		}
	}
}

bool EditorCore::Running() const {
	return editorState_.running_;
}