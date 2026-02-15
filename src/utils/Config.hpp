#pragma once
#include "../includes/Json.hpp"

#include <SDL2/SDL_pixels.h>
#include <filesystem>
#include <string>

using namespace std::string_view_literals;

namespace Tabs {
constexpr auto editor = "editor"sv;
constexpr auto font = "font"sv;
constexpr auto color = "colors"sv;
} // namespace Tabs

namespace EditorFields {
constexpr auto title = "title"sv;
constexpr auto tabSize = "tab_size"sv;
constexpr auto autoSave = "auto_save"sv;
constexpr auto autoSaveIntervalMs = "auto_save_interval_s"sv;
constexpr auto cursorBlinkMs = "cursor_blink_ms"sv;
constexpr auto wrapText = "wrap_text"sv;
constexpr auto fps = "fps"sv;
constexpr auto verticalRuler = "vertical_ruler"sv;
constexpr auto verticalRulerCount = "vertical_ruler_count"sv;
} // namespace EditorFields

namespace FontFields {
constexpr auto codeFontPath = "code_font_path"sv;
constexpr auto codeFontSize = "code_font_size"sv;
constexpr auto uiFontPath = "ui_font_path"sv;
constexpr auto uiFontSize = "ui_font_size"sv;
} // namespace FontFields

namespace ColorFields {
constexpr auto backgroundColor = "background_color"sv;
constexpr auto foregroundColor = "foreground_color"sv;
constexpr auto cursorColor = "cursor_color"sv;
constexpr auto selectionColor = "selection_color"sv;
constexpr auto verticalRulerColor = "vertical_ruler_color"sv;
} // namespace ColorFields

using Json = nlohmann::json;

class EditorConfig final {
      public:
	static EditorConfig getEditorConfig(const Json&);

	EditorConfig() = delete;
	EditorConfig(std::string title_t, int tabSize_t, bool autoSave_t, int autoSaveIntervalsMs_t, int cursorBlinkMs_t,
		     bool wrapText_t, int fps_t, bool verticalRuler_t, int verticalRulerCount_t);
	~EditorConfig() noexcept = default;

	std::string title;
	int tab_size;
	bool auto_save;
	int auto_save_intervals_ms;
	int cursor_blink_ms;
	bool wrap_text;
	int fps;

	bool vertical_ruler;
	int vertical_ruler_count;
};

class FontConfig final {
      public:
	static FontConfig getFontConfig(const Json&);

	FontConfig() = delete;
	FontConfig(std::filesystem::path codeFontPath_t, int codeFontSize_t, std::filesystem::path uiFontPath_t,
		   int uiFontSize_t);
	~FontConfig() noexcept = default;

	std::filesystem::path code_font_path;
	int code_font_size;

	std::filesystem::path ui_font_path;
	int ui_font_size;
};

class ColorsConfig final {
      public:
	static ColorsConfig getColorConfig(const Json&);

	ColorsConfig() = delete;
	ColorsConfig(const SDL_Color&, const SDL_Color&, const SDL_Color&, const SDL_Color&, const SDL_Color&);
	~ColorsConfig() noexcept = default;

	SDL_Color background_color;
	SDL_Color foreground_color;
	SDL_Color cursor_color;
	SDL_Color selection_color;
	SDL_Color vertical_ruler_color;

private:
	static SDL_Color getColorFromHex(std::string&);
};

class ConstantsConfig final {
public:
    ConstantsConfig() = default;

};

class Config final {
      public:
	Config() = delete;
	explicit Config(const std::filesystem::path&);
	~Config() noexcept;

	EditorConfig editor_;
	FontConfig font_;
	ColorsConfig colors_;
    ConstantsConfig constantConfig_;

	explicit Config(const Json&);
};