#pragma once
#include <string>
#include <filesystem>
#include <SDL2/SDL_pixels.h>

#include "../includes/Json.hpp"

using namespace std::string_view_literals;

namespace Tabs {
constexpr auto editor = "editor"sv;
constexpr auto font   = "font"sv;
constexpr auto color  = "colors"sv;
}

namespace EditorFields {
constexpr auto title              = "title"sv;
constexpr auto tabSize            = "tab_size"sv;
constexpr auto autoSave           = "auto_save"sv;
constexpr auto autoSaveIntervalMs = "auto_save_interval_s"sv;
constexpr auto cursorBlinkMs      = "cursor_blink_ms"sv;
constexpr auto wrapText = "wrap_text"sv;
constexpr auto fps = "fps"sv;
constexpr auto verticalRuler = "vertical_ruler"sv;
constexpr auto verticalRulerCount = "vertical_ruler_count"sv;
}

namespace FontFields {
constexpr auto fontPath = "font_path"sv;
constexpr auto fontSize = "font_size"sv;
}

namespace ColorFields {
constexpr auto backgroundColor = "background_color"sv;
constexpr auto foregroundColor = "foreground_color"sv;
constexpr auto cursorColor     = "cursor_color"sv;
constexpr auto selectionColor  = "selection_color"sv;
    constexpr auto verticalRulerColor = "vertical_ruler_color"sv;
}

using Json = nlohmann::json;

class EditorConfig {
public:
	static EditorConfig getEditorConfig(const Json&);

	EditorConfig() = delete;
	EditorConfig(std::string title_t, int tabSize_t, bool autoSave_t, int autoSaveIntervalsMs_t,
	    int cursorBlinkMs_t, bool wrapText_t, int fps_t, bool verticalRuler_t, int verticalRulerCount_t);
	~EditorConfig() noexcept = default;

	std::string title;
	int tab_size;
	bool        auto_save;
	int auto_save_intervals_ms;
	int cursor_blink_ms;
    bool        wrap_text;
    int fps;

    bool        vertical_ruler;
    int vertical_ruler_count;
};

class FontConfig {
public:
	static FontConfig getFontConfig(const Json&);

	FontConfig() = delete;
	FontConfig(std::filesystem::path fontPath_t, int fontSize_t);
	~FontConfig() noexcept = default;

	std::filesystem::path font_path;
	int font_size;
};

class ColorsConfig {
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

class Config final {
public:
	explicit Config(const std::filesystem::path&);

	Config() = delete;

	~Config() noexcept;

	EditorConfig editor_;
	FontConfig   font_;
	ColorsConfig colors_;

	explicit Config(const Json&);
};