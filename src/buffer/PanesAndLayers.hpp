#pragma once

#include "core/EditorCore.hpp"
#include "utils/Config.hpp"

#include <vector>

using FileId = uint_fast64_t;
using PaneId = uint_fast64_t;

struct PaneView final {
    int startX, startY;
    int endX_, endY_;
};

struct BufferView final {
    int startY_;
    int startX_;
};

class Pane final {
public:
    Pane() = delete;
    Pane(PaneView, BufferView, FileId, PaneId);
    Pane(const Pane&);
    ~Pane() noexcept = default;

    PaneView paneView_;
    BufferView textView_;

    FileId fileId_;
    PaneId paneId_;
};

enum class SplitType: char {Vertical, Horizontal, None};

class SplitNode final {
public:
    SplitNode(SplitType, bool t_isLeaf, const Pane&);
    ~SplitNode() noexcept = default;

    SplitType splitType;
    float leftChildRation = 0.5f;
    bool isLeaf;

    std::unique_ptr<SplitNode> leftChild;
    std::unique_ptr<SplitNode> rightChild;
    std::unique_ptr<Pane> pane;
};

class PanesManager final {
public:
    explicit PanesManager(const Pane&);
    ~PanesManager() noexcept = default;

    Pane getPane(PaneId);

    //void addPane(PaneView, BufferView);

    PaneId activePaneId_;
    std::unordered_map<PaneId, Pane*> paneMap_;
    SplitNode head_;
private:
    inline static PaneId paneIdCounter_{};
};

class TabLayout final {
public:
    TabLayout() = default;
    TabLayout(int activeTab_t,  int t_tabCapLines, const std::vector<std::string>& tabs_t);
    ~TabLayout() noexcept = default;

    int activeTab;
    int tabCapturedLinesOffsetY;
    std::vector<std::string> tabs;
};

class CommandLineLayout final {
public:
    CommandLineLayout() = default;
    CommandLineLayout(Modes mode_t, std::string currFileName_t, std::string t_currCommand, int cursorX_t, int cursorY_t, int charCount_t, int lineCount_t, std::string commandLineArgs_t);
    ~CommandLineLayout() noexcept = default;

    Modes mode;

    std::string currentFileName;
    std::string currentCommand;

    int cursorX, cursorY;
    int charCount, lineCount;

    std::string commandLineArgs;
};


class PanesLayout final {
public:
    PanesLayout() = default;
    PanesLayout(int t_startX, int t_startY, int t_endX, int t_endY, int t_leftDataOffsetX, const std::vector<std::string>& t_leftData, const std::vector<std::string>& t_lines);
    ~PanesLayout() noexcept = default;

    int startX, startY;
    int endX, endY;
    int leftDataOffsetX;

    std::vector<std::string> leftData;
    std::vector<std::string> lines;
};

class CursorLayout final {
public:
    CursorLayout() = default;
    CursorLayout(bool t_visible, int t_cursorX, int t_cursorY, char t_letter, PaneId);
    ~CursorLayout() noexcept = default;

    bool visible;
    int cursorX, cursorY;
    char letter;
    PaneId paneId;
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
    void addTabLayout(const FilesManager&, const ConstantsConfig&);
    void addPanesLayout(FilesManager&, const PanesManager&, const ConstantsConfig&);
    void addCursorLayout(const PanesManager&);
    void addCommandLineLayout(const CommandLineLayout&);
};