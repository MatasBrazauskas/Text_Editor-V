#pragma once
#include "includes/Json.hpp"

#include <SDL2/SDL_pixels.h>
#include <filesystem>
#include <string>

using namespace std::string_view_literals;
using Json = nlohmann::json;

const std::filesystem::path configPath = "config.json";

class WindowConfig final {
  public:
	WindowConfig() = default;
	explicit WindowConfig(const Json&);
	~WindowConfig() noexcept = default;

	std::string title;
	int width;
	int height;
	int minWidth;
	int minHeight;
	int fps_limit;

  private:
	constexpr static auto KeyTitle = "title"sv;
	constexpr static auto KeyWidth = "width"sv;
	constexpr static auto KeyHeight = "height"sv;
	constexpr static auto KeyMinWidth = "min_width"sv;
	constexpr static auto KeyMinHeight = "min_height"sv;
	constexpr static auto KeyFpsLimit = "fps_limit"sv;
};

class AutoSave final {
  public:
	AutoSave() = default;
	explicit AutoSave(const Json&);
	~AutoSave() noexcept = default;

	bool enabled;
	int interval_s;

  private:
	constexpr static auto KeyEnable = "enabled"sv;
	constexpr static auto KeyInterval_s = "interval_s"sv;
};

enum class IndentType: char { Space, Tabs };

class FeelConfig final {
  public:
	FeelConfig() = default;
	explicit FeelConfig(const Json&);
	~FeelConfig() noexcept = default;

	IndentType indentType;
	int indentSize;
	AutoSave autoSave;
	int cursorBlinkMs;
	bool wrapText;

  private:
	constexpr static auto KeyIndentType = "indent_type"sv;
	constexpr static auto KeyIndentSize = "indent_size"sv;
	constexpr static auto KeyAutoSave = "auto_save"sv;
	constexpr static auto KeyCursorBlinkMs = "cursor_blink_ms"sv;
	constexpr static auto KeyWrapText = "wrap_text"sv;
};

class VerticalRuler final {
  public:
	VerticalRuler() = default;
	explicit VerticalRuler(const Json&);
	~VerticalRuler() noexcept = default;

	bool enabled;
	int column;

  private:
	constexpr static auto KeyEnabled = "enabled"sv;
	constexpr static auto KeyColumn = "column"sv;
};

enum class LineNumberModes { None, Relative, Number };

class View final {
  public:
	View() = default;
	explicit View(const Json&);
	~View() noexcept = default;

	VerticalRuler verticalRuler;
	LineNumberModes lineNumberMode;

  private:
	constexpr static auto KeyVerticalRuler = "vertical_ruler"sv;
	constexpr static auto KeyLineNumberMode = "line_number_mode"sv;
};

class EditorConfig final {
  public:
	EditorConfig() = default;
	explicit EditorConfig(const Json&);
	~EditorConfig() noexcept = default;

	FeelConfig feel;
	View view;

  private:
	constexpr static auto KeyFeel = "feel"sv;
	constexpr static auto KeyView = "view"sv;
};

class TextFonts final {
  public:
	TextFonts() = default;
	explicit TextFonts(const Json&);
	~TextFonts() noexcept = default;

	std::string path;
	int size;

  private:
	constexpr static auto KeyPath = "path"sv;
	constexpr static auto KeySize = "size"sv;
};

class FontsConfig final {
  public:
	FontsConfig() = default;
	explicit FontsConfig(const Json&);
	~FontsConfig() noexcept = default;

	TextFonts code;
	TextFonts ui;

  private:
	constexpr static auto KeyCode = "code"sv;
	constexpr static auto KeyUi = "ui"sv;
};

class ThemeConfig final {
  public:
	ThemeConfig() = default;
	explicit ThemeConfig(const Json&);
	~ThemeConfig() noexcept = default;

	SDL_Color codeText;
	SDL_Color background;
	SDL_Color foreground;
	SDL_Color uiText;
	SDL_Color main;
	SDL_Color secondary;
	SDL_Color cursor;
	SDL_Color highlight;

  private:
	constexpr static auto KeyCodeText = "code_text"sv;
	constexpr static auto KeyBackground = "background"sv;
	constexpr static auto KeyForeground = "foreground"sv;
	constexpr static auto KeyUiText = "ui_text"sv;
	constexpr static auto KeyMain = "main"sv;
	constexpr static auto KeySecondary = "secondary"sv;
	constexpr static auto KeyCursor = "cursor"sv;
	constexpr static auto KeyHighlight = "highlight"sv;
};

class Config final {
  public:
	Config();
	~Config() noexcept = default;

	WindowConfig window;
	EditorConfig editor;
	FontsConfig fonts;
	ThemeConfig theme;

  private:
	constexpr static auto KeyWindow = "window"sv;
	constexpr static auto KeyEditor = "editor"sv;
	constexpr static auto KeyFonts = "fonts"sv;
	constexpr static auto KeyTheme = "theme"sv;
};

class CharSettings final {
  public:
	CharSettings() = default;
	~CharSettings() noexcept = default;

	int codeCharWidth, codeCharHeight;
	int uiCharWidth, uiCharHeight;
	int tabHeight;
};

class WindowSettings final {
  public:
	WindowSettings() = default;
	~WindowSettings() noexcept = default;

	int width, height;
};

class Settings final {
  public:
	explicit Settings(const Config&);
	~Settings() noexcept = default;

	constexpr static int paddingX = 25;

	CharSettings charSettings;
	WindowSettings windowSettings;
};