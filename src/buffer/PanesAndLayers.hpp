#pragma once

#include "core/EditorCore.hpp"
#include "utils/Config.hpp"

#include <vector>
#include <string_view>

using FileId = uint_fast64_t;
using strView = std::string_view;

struct PaneView final {
    int startX;
    int startY;

    int endX_;
    int endY_;
};

struct TextBufferView final {
    int startY_;
    int startX_;

    int visibleLines_;
    int visibleColumns_;
};

class Pane final {
public:
    Pane() = delete;
    Pane(PaneView, TextBufferView, FileId);
    ~Pane() noexcept = default;

    PaneView paneView_;
    TextBufferView textView_;

    FileId fileId_;
};

class PanesManager final {
public:
    PanesManager(int t_winH, int t_winW);
    ~PanesManager() noexcept = default;

    void addPane(PaneView, TextBufferView, FileId);
    void removePane(FileId);
    void setActivePane();
    void setHeightAndWidth(int height, int width);

    int windowHeight, windowWidth;

    FileId activeFileId_;
    std::vector<Pane> panes_;
};

class TabLayout final {
public:
    TabLayout() = default;
    TabLayout(int activeTab_t,  int t_tabCapLines, const std::vector<strView> tabs_t);
    ~TabLayout() noexcept = default;

    int activeTab;
    int tabCapturedLines;
    std::vector<strView> tabs;
};

class CommandLineLayout final {
public:
    CommandLineLayout() = default;
    CommandLineLayout(Modes mode_t, strView currFileName_t, strView t_currCommand, int cursorX_t, int cursorY_t, int charCount_t, int lineCount_t, strView commandLineArgs_t);
    ~CommandLineLayout() noexcept = default;

    Modes mode;

    strView currentFileName;
    strView currentCommand;

    int cursorX, cursorY;
    int charCount, lineCount;

    strView commandLineArgs;
};


class PanesLayout final {
public:
    PanesLayout() = default;
    PanesLayout(int t_startX, int t_startY, int t_endX, int t_endY, int t_leftDataOffsetX, const std::vector<strView> t_leftData, const std::vector<strView> t_lines);
    ~PanesLayout() noexcept = default;

    int startX, startY;
    int endX, endY;
    int leftDataOffsetX;

    std::vector<strView> leftData;
    std::vector<strView> lines;
};

class CursorLayout final {
public:
    CursorLayout() = default;
    CursorLayout(bool t_visible, int t_cursorX, int t_cursorY, char t_letter, const PanesLayout&);
    ~CursorLayout() noexcept = default;

    bool visible;
    int cursorX, cursorY;
    char letter;
    const PanesLayout* panesLayout;
};

class LayoutManager final {
public:
    LayoutManager() = delete;
    explicit LayoutManager(const EditorCore&, const Config&);
    ~LayoutManager() noexcept = default;

    TabLayout tabLayout;
    std::vector<PanesLayout> panesLayout;
    CursorLayout cursorLayout;
    CommandLineLayout commandLineLayout;
private:
    void addTabLayout(const Files&, const Config&);
    void addPanesLayout(const PanesLayout&);
    void addCursorLayout(const CursorLayout&);
    void addCommandLineLayout(const CommandLineLayout&);
};