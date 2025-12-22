#include "Config.hpp"
#include <fstream>
#include <stdexcept>
#include <algorithm>

EditorConfig::EditorConfig(const size_t tabSize,const bool autoSave, const size_t autoSaveIntervalMs,const size_t cursorBlinkMs, std::string title)
    : tab_size(tabSize), auto_save(autoSave), auto_save_intervals_ms(autoSaveIntervalMs), cursor_blink_ms(cursorBlinkMs), title(std::move(title)) {}

EditorConfig EditorConfig::getEditorConfig(const Json &json) {
    const auto& e = json.at(Tabs::editor);

    return EditorConfig {
        e.at(EditorFields::tabSize).get<size_t>(),
        e.at(EditorFields::autoSave).get<bool>(),
        e.at(EditorFields::autoSaveIntervalMs).get<size_t>(),
        e.at(EditorFields::cursorBlinkMs).get<size_t>(),
        e.at(EditorFields::title).get<std::string>()
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

ColorsConfig::ColorsConfig(const SDL_Color& backgroundColor, const SDL_Color& foregroundColor, const SDL_Color& cursorColor, const SDL_Color& selectionColor)
    : background_color(backgroundColor), foreground_color(foregroundColor), cursor_color(cursorColor), selection_color(selectionColor) {}

SDL_Color ColorsConfig::getColorFromHex(std::string& hexStr) {
    if (hexStr.empty() || hexStr.length() != 6) {
        throw std::runtime_error("Invalid hex string.");
    }

    bool allHexValues = std::ranges::all_of(hexStr, [](char i) {return i >= '0' && i <= 'f';});
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

ColorsConfig ColorsConfig::getColorConfig(const Json &json) {
    const auto& e = json.at(Tabs::color);

    auto bgColorStr = e.at(ColorFields::backgroundColor).get<std::string>();
    auto frColorStr = e.at(ColorFields::foregroundColor).get<std::string>();
    auto crColorStr = e.at(ColorFields::cursorColor).get<std::string>();
    auto crFrColorStr= e.at(ColorFields::selectionColor).get<std::string>();

    return ColorsConfig {
        getColorFromHex(bgColorStr),
        getColorFromHex(frColorStr),
        getColorFromHex(crColorStr),
        getColorFromHex(crFrColorStr)
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