#include "ConfigAndSettings.hpp"

#include <SDL2/SDL_ttf.h>
#include <fstream>
#include <stdexcept>

template <typename T>
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
	return {static_cast<Uint8>((v >> 16) & 0xFF), static_cast<Uint8>((v >> 8) & 0xFF), static_cast<Uint8>(v & 0xFF),
			255};
}

static LineNumberModes getLineNumber(const std::string& t_lineNumber) {
	static std::unordered_map<std::string, LineNumberModes> lineNumberModes = {
		{"none", LineNumberModes::None}, {"relative", LineNumberModes::Relative}, {"number", LineNumberModes::Number}};

	if (const auto it = lineNumberModes.find(t_lineNumber); it != lineNumberModes.end()) {
		return it->second;
	}

	throw std::runtime_error("\nCan't read the field: line_number_mode");
}

static IndentType getIndentType(const std::string& t_indentType) {
	static std::unordered_map<std::string, IndentType> indentTypeMap = {{"space", IndentType::Space},
																		{"tabs", IndentType::Tabs}};

	if (const auto it = indentTypeMap.find(t_indentType); it != indentTypeMap.end()) {
		return it->second;
	}

	throw std::runtime_error("\nCan't read the field: indent_type");
}

WindowConfig::WindowConfig(const Json& t_json) {
	try {
		title = getJsonObject<std::string>(t_json, KeyTitle);
		width = getJsonObject<int>(t_json, KeyWidth);
		height = getJsonObject<int>(t_json, KeyHeight);
		minWidth = getJsonObject<int>(t_json, KeyMinWidth);
		minHeight = getJsonObject<int>(t_json, KeyMinHeight);
		fps_limit = getJsonObject<int>(t_json, KeyFpsLimit);
	} catch (std::exception& e) {
		throw std::runtime_error("\nJSON error parsing WindowConfig tab: " + std::string{e.what()});
	}
}

AutoSave::AutoSave(const Json& t_json) {
	try {
		enabled = getJsonObject<bool>(t_json, KeyEnable);
		interval_s = getJsonObject<int>(t_json, KeyInterval_s);
	} catch (std::exception& e) {
		throw std::runtime_error("\nJSON error parsing AutoSave tab: " + std::string{e.what()});
	}
}

FeelConfig::FeelConfig(const Json& t_json) {
	try {
		const auto indentTypeStr = getJsonObject<std::string>(t_json, KeyIndentType);
		indentType = getIndentType(indentTypeStr);
		indentSize = getJsonObject<int>(t_json, KeyIndentSize);
		autoSave = AutoSave(getJsonObject<Json>(t_json, KeyAutoSave));
		cursorBlinkMs = getJsonObject<int>(t_json, KeyCursorBlinkMs);
		wrapText = getJsonObject<bool>(t_json, KeyWrapText);
	} catch (std::exception& e) {
		throw std::runtime_error("\nJSON error parsing FeelConfig tab: " + std::string{e.what()});
	}
}

VerticalRuler::VerticalRuler(const Json& t_json) {
	try {
		enabled = getJsonObject<int>(t_json, KeyEnabled);
		column = getJsonObject<int>(t_json, KeyColumn);
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

EditorConfig::EditorConfig(const Json& t_json) {
	try {
		feel = FeelConfig{getJsonObject<Json>(t_json, KeyFeel)};
		view = View{getJsonObject<Json>(t_json, KeyView)};
	} catch (std::exception& e) {
		throw std::runtime_error("\nJSON error parsing EditorConfig tab: " + std::string{e.what()});
	}
}

TextFonts::TextFonts(const Json& t_json) {
	try {
		path = getJsonObject<std::string>(t_json, KeyPath);
		size = getJsonObject<int>(t_json, KeySize);
	} catch (std::exception& e) {
		throw std::runtime_error("\nJSON error parsing TextFonts tab: " + std::string{e.what()});
	}
}

FontsConfig::FontsConfig(const Json& t_json) {
	try {
		const Json code_Json = getJsonObject<Json>(t_json, KeyCode);
		const Json ui_Json = getJsonObject<Json>(t_json, KeyUi);

		code = TextFonts(code_Json);
		ui = TextFonts(ui_Json);
	} catch (std::exception& e) {
		throw std::runtime_error("\nJSON error parsing TextFonts tab: " + std::string{e.what()});
	}
}

ThemeConfig::ThemeConfig(const Json& t_json) {
	try {
		const auto codeTextObj = getJsonObject<std::string>(t_json, KeyCodeText);
		const auto backgroundObj = getJsonObject<std::string>(t_json, KeyBackground);
		const auto foregroundObj = getJsonObject<std::string>(t_json, KeyForeground);
		const auto uiTextObj = getJsonObject<std::string>(t_json, KeyUiText);
		const auto mainObj = getJsonObject<std::string>(t_json, KeyMain);
		const auto secondaryObj = getJsonObject<std::string>(t_json, KeySecondary);
		const auto cursorObj = getJsonObject<std::string>(t_json, KeyCursor);
		const auto highlightObj = getJsonObject<std::string>(t_json, KeyHighlight);

		codeText = HexToSDL(codeTextObj);
		background = HexToSDL(backgroundObj);
		foreground = HexToSDL(foregroundObj);
		uiText = HexToSDL(uiTextObj);
		main = HexToSDL(mainObj);
		secondary = HexToSDL(secondaryObj);
		cursor = HexToSDL(cursorObj);
		highlight = HexToSDL(highlightObj);
	} catch (std::exception& e) {
		throw std::runtime_error("\nJSON error parsing ThemeConfig tab: " + std::string{e.what()});
	}
}

Config::Config() : window{}, editor{}, fonts{}, theme{} {
	if (!std::filesystem::exists(configPath)) {
		throw std::runtime_error("ConfigAndSettings path must be present.");
	}

	std::ifstream configFile(configPath);
	Json json;
	configFile >> json;

	try {
		window = WindowConfig{getJsonObject<Json>(json, KeyWindow)};
		editor = EditorConfig{getJsonObject<Json>(json, KeyEditor)};
		fonts = FontsConfig{getJsonObject<Json>(json, KeyFonts)};
		theme = ThemeConfig{getJsonObject<Json>(json, KeyTheme)};
	} catch (const std::exception& e) {
		throw std::runtime_error("JSON Parse Failure: " + std::string{e.what()});
	}
}

Settings::Settings(const Config& t_config) : charSettings{}, windowSettings{} {
	TTF_Init();

	const auto& codeFont = t_config.fonts.code;
	const auto& uiFont = t_config.fonts.ui;

	const auto codePath = codeFont.path.c_str();
	const auto uiPath = uiFont.path.c_str();

	const auto codeFont_ = TTF_OpenFont(codePath, codeFont.size);
	const auto uiFont_ = TTF_OpenFont(uiPath, uiFont.size);

	TTF_SetFontHinting(codeFont_, TTF_HINTING_MONO);
	TTF_SetFontKerning(codeFont_, 0);

	TTF_SizeText(codeFont_, "A", &charSettings.codeCharWidth, &charSettings.codeCharHeight);

	TTF_SetFontHinting(uiFont_, TTF_HINTING_MONO);
	TTF_SetFontKerning(uiFont_, 0);

	TTF_SizeText(uiFont_, "A", &charSettings.uiCharWidth, &charSettings.uiCharHeight);

	charSettings.tabHeight = charSettings.uiCharHeight + 5;

	windowSettings.width = t_config.window.width;
	windowSettings.height = t_config.window.height;
}