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
    constexpr auto tabSize= "tab_size"sv;
    constexpr auto autoSave = "auto_save"sv;
    constexpr auto autoSaveIntervalMs = "auto_save_interval_ms"sv;
    constexpr auto cursorBlinkMs = "cursor_blink_ms"sv;
}

namespace FontFields {
    constexpr auto fontPath= "font_path"sv;
    constexpr auto fontSize = "font_size"sv;
}

namespace  ColorFields {
    constexpr auto backgroundColor = "background_color"sv;
    constexpr auto foregroundColor = "foreground_color"sv;
    constexpr auto cursorColor = "cursor_color"sv;
    constexpr auto selectionColor = "selection_color"sv;
}

using Json = nlohmann::json;

class EditorConfig {
public:
    static EditorConfig getEditorConfig(const Json& json);
    EditorConfig() = default;
    ~EditorConfig() noexcept = default;
//private:
    size_t tab_size;
    bool auto_save;
    size_t auto_save_intervals_ms;
    size_t cursor_blink_ms;
    EditorConfig(size_t tabSize, bool autoSave, size_t autoSaveIntervalMs, size_t cursorBlinkMs);
};

class FontConfig {
public:
    static FontConfig getFontConfig(const Json& json);
    FontConfig() = default;
    ~FontConfig() noexcept = default;
//private:
    std::string font_path;
    size_t font_size{};
    FontConfig(std::string fontPath, size_t fontSize);
};

class ColorsConfig {
public:
    static ColorsConfig getColorConfig(const Json& json);
    ColorsConfig() = default;
    ~ColorsConfig() noexcept = default;
//private:
    SDL_Color background_color;
    SDL_Color foreground_color;
    SDL_Color cursor_color;
    SDL_Color selection_color;
private:
    ColorsConfig(const SDL_Color& backgroundColor, const SDL_Color& foregroundColor, const SDL_Color& cursorColor, const SDL_Color& selectionColor);
    static SDL_Color getColorFromHex(std::string& hexStr);
};

class Config {
public:
    Config(const std::filesystem::path& configPath);
    Config() = delete;
    ~Config() noexcept;
//private:
    EditorConfig editor_;
    FontConfig font_;
    ColorsConfig colors_;

    explicit Config(const Json& json);
};