#pragma once
#include "../includes/Json.hpp"

#include <SDL2/SDL_pixels.h>
#include <filesystem>
#include <string>

using namespace std::string_view_literals;
using Json = nlohmann::json;

const std::filesystem::path configPath = "config.json";

class Window final {
  public:
	Window() = default;
	explicit Window(const Json&);
	~Window() noexcept = default;

	std::string title;
	int width;
	int height;
	int fps_limit;

  private:
	constexpr static auto Title = "title"sv;
	constexpr static auto Width = "width"sv;
	constexpr static auto Height = "height"sv;
	constexpr static auto FpsLimit = "fps_limit"sv;
};

class AutoSave final {
  public:
	AutoSave() = default;
	explicit AutoSave(const Json&);
	~AutoSave() noexcept = default;

	bool enabled;
	int interval_s;

  private:
	constexpr static auto Enable = "enabled"sv;
	constexpr static auto Interval_s = "interval_s"sv;
};

class Feel final {
  public:
	Feel() = default;
	explicit Feel(const Json&);
	~Feel() noexcept = default;

	int tabSize;
	AutoSave autoSave;
	int cursorBlinkMs;
	bool wrapText;

  private:
	constexpr static auto TabSize = "tab_size"sv;
	constexpr static auto KeyAutoSave = "auto_save"sv;
	constexpr static auto CursorBlinkMs = "cursor_blink_ms"sv;
	constexpr static auto WrapText = "wrap_text"sv;
};

class VerticalRuler final {
  public:
	VerticalRuler() = default;
	explicit VerticalRuler(const Json&);
	~VerticalRuler() noexcept = default;

	bool enabled;
	int column;

  private:
	constexpr static auto Enabled = "enabled"sv;
	constexpr static auto Column = "column"sv;
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

class Editor final {
  public:
	Editor() = default;
	explicit Editor(const Json&);
	~Editor() noexcept = default;

	Feel feel;
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
	constexpr static auto Path = "path"sv;
	constexpr static auto Size = "size"sv;
};

class Fonts final {
  public:
	Fonts() = default;
	explicit Fonts(const Json&);
	~Fonts() noexcept = default;

	TextFonts code;
	TextFonts ui;

  private:
	constexpr static auto Code = "code"sv;
	constexpr static auto Ui = "ui"sv;
};

class Theme final {
  public:
	Theme() = default;
	explicit Theme(const Json&);
	~Theme() noexcept = default;

	SDL_Color codeText;
	SDL_Color background;
	SDL_Color foreground;
	SDL_Color uiText;
	SDL_Color main;
	SDL_Color secondary;
	SDL_Color cursor;
	SDL_Color highlight;

  private:
	constexpr static auto CodeText = "code_text"sv;
	constexpr static auto Background = "background"sv;
	constexpr static auto Foreground = "foreground"sv;
	constexpr static auto UiText = "ui_text"sv;
	constexpr static auto Main = "main"sv;
	constexpr static auto Secondary = "secondary"sv;
	constexpr static auto Cursor = "cursor"sv;
	constexpr static auto Highlight = "highlight"sv;
};

class Config final {
  public:
	Config();
	~Config() noexcept = default;

	Window window;
	Editor editor;
	Fonts fonts;
	Theme theme;

  private:
	constexpr static auto KeyWindow = "window"sv;
	constexpr static auto KeyEditor = "editor"sv;
	constexpr static auto KeyFonts = "fonts"sv;
	constexpr static auto KeyTheme = "theme"sv;
};

class Settings final {
public:
	explicit Settings(const Config&);
	~Settings() noexcept = default;

	int codeCharWidth{}, codeCharHeight{};
	int uiCharWidth{}, uiCharHeight{};

	int tabHeight{};
	int paddingX = 25;
};