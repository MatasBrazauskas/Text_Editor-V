#pragma once
#include <string>
#include "../includes//Json.hpp"

namespace Wrapper {

    namespace Tabs {
        constexpr std::string_view editor = "editor";
        constexpr std::string_view font   = "font";
        constexpr std::string_view color  = "colors";
    }

    namespace EditorFields {
        constexpr std::string_view tabSize= "tab_size";
        constexpr std::string_view autoSave = "auto_save";
        constexpr std::string_view autoSaveIntervalMs = "auto_save_interval_ms";
        constexpr std::string_view cursorBlinkMs = "cursor_blink_ms";
    }

    namespace FontFields {
        constexpr std::string_view fontPath= "font_path";
        constexpr std::string_view fontSize = "font_size";
    }

    namespace  ColorFields {
        constexpr std::string_view backgroundColor = "background_color";
        constexpr std::string_view foregroundColor = "foreground_color";
        constexpr std::string_view cursorColor = "cursor_color";
        constexpr std::string_view selectionColor = "selection_color";
    }

    using Json = nlohmann::json;

    class EditorConfig {
    public:
        static EditorConfig getEditorConfig(const Json& json);
        EditorConfig() = default;
        ~EditorConfig() noexcept = default;
    private:
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
    private:
        std::string font_path;
        size_t font_size;
        FontConfig(const std::string &fontPath, size_t fontSize);
    };

    class ColorsConfig {
    public:
        static ColorsConfig getColorConfig(const Json& json);
        ColorsConfig() = default;
        ~ColorsConfig() noexcept = default;
    private:
        std::string background_color;
        std::string foreground_color;
        std::string cursor_color;
        std::string selection_color;
        ColorsConfig(const std::string &backgroundColor, const std::string &foregroundColor, const std::string &cursorColor, const std::string &selectionColor);
    };

    class Config {
    public:
        explicit Config(const std::string& configPath);
        Config() = delete;
        ~Config() noexcept;
    private:
        EditorConfig editor_;
        FontConfig font_;
        ColorsConfig colors_;
    };
}