#include "Config.hpp"

#include <fstream>
#include <stdexcept>

using namespace Wrapper;

EditorConfig::EditorConfig(const size_t tabSize,const bool autoSave, const size_t autoSaveIntervalMs,const size_t cursorBlinkMs)
    : tab_size(tabSize), auto_save(autoSave), auto_save_intervals_ms(autoSaveIntervalMs), cursor_blink_ms(cursorBlinkMs) {}

EditorConfig EditorConfig::getEditorConfig(const Json &json) {
    const auto& e = json.at(Tabs::editor);

    return EditorConfig {
        e.at(EditorFields::tabSize).get<size_t>(),
        e.at(EditorFields::autoSave).get<bool>(),
        e.at(EditorFields::autoSaveIntervalMs).get<size_t>(),
        e.at(EditorFields::cursorBlinkMs).get<size_t>()
    };
}

FontConfig::FontConfig(const std::string& fontPath, const size_t fontSize)
    : font_path(fontPath), font_size(fontSize) {}

FontConfig FontConfig::getFontConfig(const Json &json) {
    const auto& e = json.at(Tabs::font);

    return FontConfig{
        e.at(FontFields::fontPath).get<std::string>(),
        e.at(FontFields::fontSize).get<size_t>()
    };
}

ColorsConfig::ColorsConfig(const std::string& backgroundColor,const std::string& foregroundColor,const std::string& cursorColor,const std::string& selectionColor)
    : background_color(backgroundColor), foreground_color(foregroundColor), cursor_color(cursorColor), selection_color(selectionColor) {}

ColorsConfig ColorsConfig::getColorConfig(const Json &json) {
    const auto& e = json.at(Tabs::color);

    return ColorsConfig{
        e.at(ColorFields::backgroundColor).get<std::string>(),
        e.at(ColorFields::foregroundColor).get<std::string>(),
        e.at(ColorFields::cursorColor).get<std::string>(),
        e.at(ColorFields::selectionColor).get<std::string>()
    };
}

Config::Config(const std::string& configPath): editor_(), font_(), colors_() {
    if (configPath.empty()) {
        throw std::runtime_error("Config path must be non-empty.");
    }

    std::ifstream configFile(configPath);

    Json json;
    configFile >> json;

    editor_ =EditorConfig::getEditorConfig(json);
    font_ = FontConfig::getFontConfig(json);
    colors_ = ColorsConfig::getColorConfig(json);
}

Config::~Config() noexcept = default;