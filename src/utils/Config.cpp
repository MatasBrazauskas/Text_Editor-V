#include "Config.hpp"
#include <fstream>
#include <stdexcept>

using namespace Utils;

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

FontConfig::FontConfig(std::string fontPath, const size_t fontSize)
    : font_path(std::move(fontPath)), font_size(fontSize) {}

FontConfig FontConfig::getFontConfig(const Json &json) {
    const auto& e = json.at(Tabs::font);

    return FontConfig{
        e.at(FontFields::fontPath).get<std::string>(),
        e.at(FontFields::fontSize).get<size_t>()
    };
}

ColorsConfig::ColorsConfig(std::string backgroundColor,std::string foregroundColor,std::string cursorColor,std::string selectionColor)
    : background_color(std::move(backgroundColor)), foreground_color(std::move(foregroundColor)), cursor_color(std::move(cursorColor)), selection_color(std::move(selectionColor)) {}

ColorsConfig ColorsConfig::getColorConfig(const Json &json) {
    const auto& e = json.at(Tabs::color);

    return ColorsConfig{
        e.at(ColorFields::backgroundColor).get<std::string>(),
        e.at(ColorFields::foregroundColor).get<std::string>(),
        e.at(ColorFields::cursorColor).get<std::string>(),
        e.at(ColorFields::selectionColor).get<std::string>()
    };
}

static Json loadJson(const std::string &configPath) {
    if (configPath.empty()) {
        throw std::runtime_error("Config path must be non-empty.");
    }

    std::ifstream configFile(configPath);

    Json json;
    configFile >> json;
    return json;
}

Config::Config(const std::filesystem::path& configPath) : Config(loadJson(configPath)) {}

Config::Config(const Json &json) :
    editor_(EditorConfig::getEditorConfig(json)),
    font_(FontConfig::getFontConfig(json)),
    colors_(ColorsConfig::getColorConfig(json))
{}

Config::~Config() noexcept = default;