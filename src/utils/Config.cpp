#include "Config.hpp"

#include <fstream>
#include <stdexcept>

template<typename T>
static T getJsonObject(const Json& t_jsonObject, std::string_view t_name) {
	if (const auto it = t_jsonObject.find(t_name); it != t_jsonObject.end() && !it.value().is_null()) {
		return it.value().get<T>();
	}

	throw std::runtime_error("\nCan't read the field: " + std::string{t_name});
}

static SDL_Color HexToSDL(std::string hex) {
	if (hex[0] == '#')
		hex.erase(0, 1);

	const uint32_t v = std::stoul(hex, nullptr, 16);
	return {
		static_cast<Uint8>((v >> 16) & 0xFF),
		static_cast<Uint8>((v >> 8) & 0xFF),
		static_cast<Uint8>(v & 0xFF),
		255
	};
}

static LineNumberModes getLineNumber(const std::string& t_lineNumber) {
	static std::unordered_map<std::string, LineNumberModes> lineNumberModes = {
		{"none", LineNumberModes::None},
		{"relative", LineNumberModes::Relative},
		{"number", LineNumberModes::Number}
	};

	if (const auto it = lineNumberModes.find(t_lineNumber); it != lineNumberModes.end()) {
		return it->second;
	}

	throw std::runtime_error("\nCan't read the field: line_number_modes");
}

Window::Window(const Json& t_json) {
	try {
		title = getJsonObject<std::string>(t_json, Title);
		width = getJsonObject<int>(t_json, Width);
		height = getJsonObject<int>(t_json, Height);
		centered = getJsonObject<bool>(t_json, Centered);
		fps_limit = getJsonObject<int>(t_json, FpsLimit);
	} catch (std::exception& e) {
		throw std::runtime_error("\nJSON error parsing Window tab: " + std::string{e.what()});
	}
}

AutoSave::AutoSave(const Json& t_json) {
	try {
		enabled = getJsonObject<bool>(t_json, Enable);
		interval_s = getJsonObject<int>(t_json, Interval_s);
	} catch (std::exception& e) {
		throw std::runtime_error("\nJSON error parsing AutoSave tab: " + std::string{e.what()});
	}
}

Feel::Feel(const Json& t_json) {
	try {
		tabSize = getJsonObject<int>(t_json, TabSize);
		autoSave = AutoSave(getJsonObject<Json>(t_json, KeyAutoSave));
		cursorBlinkMs = getJsonObject<int>(t_json, CursorBlinkMs);
		wrapText = getJsonObject<bool>(t_json, WrapText);
	} catch (std::exception& e) {
		throw std::runtime_error("\nJSON error parsing Feel tab: " + std::string{e.what()});
	}
}

VerticalRuler::VerticalRuler(const Json& t_json) {
	try {
		enabled = getJsonObject<int>(t_json, Enabled);
		column = getJsonObject<int>(t_json, Column);
	} catch (std::exception& e) {
		throw std::runtime_error("\nJSON error parsing VerticalRuler tab: " + std::string{e.what()});
	}
}

View::View(const Json& t_json) {
	try {
		verticalRuler = VerticalRuler{getJsonObject<Json>(t_json, KeyVerticalRuler)};
		const auto lineModeStr = getJsonObject<std::string>(t_json, KeyLineNumberMode);
		lineNumberMode = getLineNumber(lineModeStr);
	} catch (std::exception& e) {
		throw std::runtime_error("\nJSON error parsing View tab: " + std::string{e.what()});
	}
}

Editor::Editor(const Json& t_json) {
	try {
		feel = Feel{getJsonObject<Json>(t_json, KeyFeel)};
		view = View{getJsonObject<Json>(t_json, KeyView)};
	} catch (std::exception& e) {
		throw std::runtime_error("\nJSON error parsing Editor tab: " + std::string{e.what()});
	}
}

TextFonts::TextFonts(const Json& t_json) {
	try {
		path = getJsonObject<std::string>(t_json, Path);
		size = getJsonObject<int>(t_json, Size);
	} catch (std::exception& e) {
		throw std::runtime_error("\nJSON error parsing TextFonts tab: " + std::string{e.what()});
	}
}

Fonts::Fonts(const Json& t_json) {
	try {
		const Json code_Json = getJsonObject<Json>(t_json, Code);
		const Json ui_Json = getJsonObject<Json>(t_json, Ui);

		code = TextFonts(code_Json);
		ui = TextFonts(ui_Json);
	} catch (std::exception& e) {
		throw std::runtime_error("\nJSON error parsing TextFonts tab: " + std::string{e.what()});
	}
}

Theme::Theme(const Json& t_json) {
	try {
		const auto codeTextObj = getJsonObject<std::string>(t_json, CodeText);
		const auto backgroundObj = getJsonObject<std::string>(t_json,Background);
		const auto foregroundObj = getJsonObject<std::string>(t_json, Foreground);
		const auto uiTextObj = getJsonObject<std::string>(t_json, UiText);
		const auto mainObj = getJsonObject<std::string>(t_json, Main);
		const auto secondaryObj = getJsonObject<std::string>(t_json, Secondary);
		const auto cursorObj = getJsonObject<std::string>(t_json, Cursor);
		const auto highlightObj = getJsonObject<std::string>(t_json, Highlight);

		codeText = HexToSDL(codeTextObj);
		background = HexToSDL(backgroundObj);
		foreground = HexToSDL(foregroundObj);
		uiText = HexToSDL(uiTextObj);
		main = HexToSDL(mainObj);
		secondary = HexToSDL(secondaryObj);
		cursor = HexToSDL(cursorObj);
		highlight = HexToSDL(highlightObj);
	} catch (std::exception& e) {
		throw std::runtime_error("\nJSON error parsing Theme tab: " + std::string{e.what()});
	}
}

Config::Config(const std::filesystem::path& t_path):window{},editor{},fonts{}, theme{} {
	if (!std::filesystem::exists(t_path)) {
		throw std::runtime_error("Config path must be present.");
	}

	std::ifstream configFile(t_path);
	Json json;
	configFile >> json;

	try {
		window = Window{getJsonObject<Json>(json, KeyWindow)};
		editor = Editor{getJsonObject<Json>(json, KeyEditor)};
		fonts = Fonts{getJsonObject<Json>(json, KeyFonts)};
		theme = Theme{getJsonObject<Json>(json, KeyTheme)};
	} catch (const std::exception& e) {
		throw std::runtime_error("JSON Parse Failure: " + std::string{e.what()});
	}
}