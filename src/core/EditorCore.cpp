#include "EditorCore.hpp"

#include "utils/ConfigAndSettings.hpp"

#include <SDL.h>

EditorState::EditorState() : currentMode_{Modes::Normal}, running_{true} {}

EditorInputAndOutput::EditorInputAndOutput() : cursorIndexX{}, commandLineState_{CommandLineState::None} {}

void EditorInputAndOutput::cleanInputs() {
	input_.clear();

	cursorIndexX = 0;
	commandLineMessage_.clear();
	commandLineState_ = CommandLineState::None;
}

void EditorInputAndOutput::removeLastInputChar() {
	input_.pop_back();
	commandLineMessage_.pop_back();

	cursorIndexX = std::min(static_cast<int>(commandLineMessage_.size()), cursorIndexX);
}

void EditorInputAndOutput::setError(const std::string t_error) {
	cursorIndexX = 0;
	commandLineMessage_ = t_error;
	commandLineState_ = CommandLineState::Error;
}

void EditorInputAndOutput::setInfo(const std::string t_info) {
	cursorIndexX = 0;
	commandLineMessage_ = t_info;
	commandLineState_ = CommandLineState::Info;
}

EditorCore::EditorCore(const int argc, char** argv, Settings& t_settings) : filesManager_{argc, argv}, settings_{t_settings} {
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

void EditorCore::HandleKeyboardInput(const Config& t_config) {
	SDL_Event event;
	auto winSettings = settings_.windowSettings;

	while (SDL_PollEvent(&event)) {
		const auto input = EncodeInput(event);

		if (std::holds_alternative<SpecialCases>(input)) {
			const auto specialCase = std::get<SpecialCases>(input);
			HandleSpecialCases(specialCase, event);
		} else {
			const auto str = std::get<std::string>(input);

			if (str.empty()) {
				return;
			}

			editorInputAndOutput_.input_.append(str);

			auto& file = filesManager_.getFile();
			auto& cursor = panesManager_.getCurrPane().getCursor();

			switch (editorState_.currentMode_) {
			case Modes::Normal:
				normalMode_.HandleKeyboardInput(file, cursor, editorState_, editorInputAndOutput_);
				break;
			case Modes::Insert:
				insertMode_.HandleKeyboardInput(editorInputAndOutput_, file, cursor, t_config);
				break;
			case Modes::Command:
				commandMode_.HandleKeyboardInput(editorState_, editorInputAndOutput_, filesManager_, panesManager_, str.back());
				break;
			case Modes::WindowMode:
				windowSubCommand_.ExecuteCommand(panesManager_, winSettings, editorInputAndOutput_.input_.back());
				break;
			case Modes::FileMode:
				fileSubCommand_.ExecuteCommand(panesManager_, filesManager_, winSettings, editorInputAndOutput_.input_.back());
				break;
			}
		}
	}
}

bool EditorCore::Running() const {
	return editorState_.running_;
}