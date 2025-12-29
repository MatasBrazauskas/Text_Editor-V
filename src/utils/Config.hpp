#pragma once
#include <string>
#include <filesystem>
#include "../includes/Json.hpp"
#include <SDL_pixels.h>

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
constexpr auto autoSaveIntervalMs = "auto_save_interval_ms"sv;
constexpr auto cursorBlinkMs      = "cursor_blink_ms"sv;
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
}

using Json = nlohmann::json;

class EditorConfig {
public:
	static EditorConfig getEditorConfig(const Json&);

	EditorConfig() = default;

	EditorConfig(size_t, bool, size_t, size_t, std::string);

	~EditorConfig() noexcept = default;

	size_t      tab_size;
	bool        auto_save;
	size_t      auto_save_intervals_ms;
	size_t      cursor_blink_ms;
	std::string title;
};

class FontConfig {
public:
	static FontConfig getFontConfig(const Json&);

	FontConfig() = default;

	~FontConfig() noexcept = default;

	std::string font_path;
	size_t      font_size{};

	FontConfig(std::string, size_t);
};

class ColorsConfig {
public:
	static ColorsConfig getColorConfig(const Json&);

	ColorsConfig() = default;

	~ColorsConfig() noexcept = default;

	SDL_Color background_color;
	SDL_Color foreground_color;
	SDL_Color cursor_color;
	SDL_Color selection_color;

private:
	ColorsConfig(const SDL_Color&, const SDL_Color&, const SDL_Color&, const SDL_Color&);

	static SDL_Color getColorFromHex(std::string&);
};

class Config final {
public:
	Config(const std::filesystem::path&);

	Config() = delete;

	~Config() noexcept;

	EditorConfig editor_;
	FontConfig   font_;
	ColorsConfig colors_;

	explicit Config(const Json&);
};