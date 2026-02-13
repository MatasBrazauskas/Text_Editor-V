#pragma once

#include <vector>
#include <string_view>

#include "../core/Editor.hpp"

using FileId = uint_fast64_t;

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

class Panes final {
public:
    Panes();
    ~Panes() noexcept = default;

    void addPane(PaneView, TextBufferView, FileId);
    void removePane(FileId);

    FileId activeFileId_;
    std::vector<Pane> panes_;
};

using strView = std::string_view;

class TabLayout final {
public:
    TabLayout() = default;
    TabLayout(int activeTab_t,  int t_tabCapLines, const std::vector<strView>& tabs_t);
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
    PanesLayout(int t_startX, int t_startY, int t_endX, int t_endY, int t_leftDataOffsetX, const std::vector<strView>& t_leftData, const std::vector<strView>& t_lines);
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
    LayoutManager(int t_windowWidth, int t_windowHeight);
    ~LayoutManager() noexcept = default;

    void addTabLayout(const TabLayout&);
    void addPanesLayout(const PanesLayout&);
    void addCursorLayout(const CursorLayout&);
    void addCommandLineLayout(const CommandLineLayout&);

    int windowWidth, windowHeight;

    TabLayout tabLayout;
    std::vector<PanesLayout> panesLayout;
    CursorLayout cursorLayout;
    CommandLineLayout commandLineLayout;
};