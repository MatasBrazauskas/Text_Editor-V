import std::string;

struct EditorConfig {
    size_t tab_size;
    bool auto_save;
    size_t auto_save_intervals_ms;
    size_t cursor_blink_ms;
    bool indent_with_spaces;
}

struct FontConfig {
    std::string fond_path;
    size_t font_size;
}

struct ColorConfig {
    std::string background;
    std::string foreground;
    std::string cursor;
    std::string selection;
}

struct Config {
    EditorConfig editor;
    FontConfig font;
    ColorConfig colors;
};