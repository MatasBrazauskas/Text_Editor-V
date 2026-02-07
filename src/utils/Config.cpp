#include "Config.hpp"

#include <algorithm>
#include <fstream>
#include <stdexcept>

EditorConfig::EditorConfig(std::string title_t, const int tabSize_t, const bool autoSave_t,
			   const int autoSaveIntervalsMs_t, const int cursorBlinkMs_t, const bool wrapText_t,
			   const int fps_t, const bool verticalRuler_t, const int verticalRulerCount_t)
    : title{std::move(title_t)}, tab_size{tabSize_t}, auto_save{autoSave_t},
      auto_save_intervals_ms(autoSaveIntervalsMs_t), cursor_blink_ms{cursorBlinkMs_t}, wrap_text{wrapText_t}, fps{fps_t},
      vertical_ruler{verticalRuler_t}, vertical_ruler_count{verticalRulerCount_t} {}

EditorConfig EditorConfig::getEditorConfig(const Json& json) {
	const auto& e = json.at(Tabs::editor);

	return EditorConfig{
	    e.at(EditorFields::title).get<std::string>(),
	    e.at(EditorFields::tabSize).get<int>(),
	    e.at(EditorFields::autoSave).get<bool>(),
	    e.at(EditorFields::autoSaveIntervalMs).get<int>(),
	    e.at(EditorFields::cursorBlinkMs).get<int>(),
	    e.at(EditorFields::wrapText).get<bool>(),
	    e.at(EditorFields::fps).get<int>(),
	    e.at(EditorFields::verticalRuler).get<bool>(),
	    e.at(EditorFields::verticalRulerCount).get<int>(),
	};
}

FontConfig::FontConfig(std::filesystem::path fontPath_t, const int fontSize_t, std::filesystem::path uiFondPath_t,
		       const int uiFontSize)
    : code_font_path{std::move(fontPath_t)}, code_font_size{fontSize_t}, ui_font_path{std::move(uiFondPath_t)},
      ui_font_size{uiFontSize} {}

FontConfig FontConfig::getFontConfig(const Json& json) {
	const auto& e = json.at(Tabs::font);

	return FontConfig{
	    e.at(FontFields::codeFontPath).get<std::filesystem::path>(), e.at(FontFields::codeFontSize).get<int>(),
	    e.at(FontFields::uiFontPath).get<std::filesystem::path>(), e.at(FontFields::uiFontSize).get<int>()};
}

ColorsConfig::ColorsConfig(const SDL_Color& backgroundColor_t, const SDL_Color& foregroundColor_t,
			   const SDL_Color& cursorColor_t, const SDL_Color& selectionColor_t,
			   const SDL_Color& verticalRulerColor_t)
    : background_color{backgroundColor_t}, foreground_color{foregroundColor_t}, cursor_color{cursorColor_t},
      selection_color{selectionColor_t}, vertical_ruler_color{verticalRulerColor_t} {}

SDL_Color ColorsConfig::getColorFromHex(std::string& hexStr) {
	if (hexStr.empty() || hexStr.length() != 6) {
		throw std::runtime_error("Invalid hex string.");
	}

	const bool allHexValues = std::ranges::all_of(hexStr, [](char i) { return i >= '0' && i <= 'f'; });
	if (!allHexValues) {
		throw std::runtime_error("Invalid hex string.");
	}

	uint8_t rgbColors[3];
	for (auto i{0zu}; i < 3; i++) {
		const auto rgbColor = hexStr.substr(i * 2, 2);
		rgbColors[i] = static_cast<uint8_t>(std::stoi(rgbColor, nullptr, 16));
	}

	return {rgbColors[0], rgbColors[1], rgbColors[2]};
}

ColorsConfig ColorsConfig::getColorConfig(const Json& json) {
	const auto& e = json.at(Tabs::color);

	auto bgColorStr = e.at(ColorFields::backgroundColor).get<std::string>();
	auto frColorStr = e.at(ColorFields::foregroundColor).get<std::string>();
	auto crColorStr = e.at(ColorFields::cursorColor).get<std::string>();
	auto crFrColorStr = e.at(ColorFields::selectionColor).get<std::string>();
	auto vrColorStr = e.at(ColorFields::verticalRulerColor).get<std::string>();

	return ColorsConfig{getColorFromHex(bgColorStr), getColorFromHex(frColorStr), getColorFromHex(crColorStr),
			    getColorFromHex(crFrColorStr), getColorFromHex(vrColorStr)};
}

static Json loadJson(const std::string& configPath) {
	if (configPath.empty()) {
		throw std::runtime_error("Config path must be non-empty.");
	}

	std::ifstream configFile(configPath);

	Json json;
	configFile >> json;
	return json;
}

Config::Config(const std::filesystem::path& configPath) : Config(loadJson(configPath)) {}

Config::Config(const Json& json)
    : editor_(EditorConfig::getEditorConfig(json)), font_(FontConfig::getFontConfig(json)),
      colors_(ColorsConfig::getColorConfig(json)) {}

Config::~Config() noexcept = default;