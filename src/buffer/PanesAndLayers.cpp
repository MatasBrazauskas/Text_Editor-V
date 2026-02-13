#include "PanesAndLayers.hpp"

Pane::Pane(const PaneView paneView_t, const TextBufferView textView_t, const FileId fileId_t)
    : paneView_{paneView_t}, textView_{textView_t}, fileId_{fileId_t} {}

Panes::Panes() : activeFileId_{} {}

void Panes::addPane(const PaneView paneView_t, const TextBufferView textView_t, const FileId fileId_t) {
    panes_.emplace_back(paneView_t, textView_t, fileId_t);
}

void Panes::removePane(const FileId fileId_t) {
    const auto predicate = [fileId_t](const Pane& pane) { return pane.fileId_ == fileId_t; };
    std::erase_if(panes_, predicate);
}

TabLayout::TabLayout(const int activeTab_t, const int t_tabCapLines,  const std::vector<strView>& tabs_t)
    : activeTab{activeTab_t}, tabCapturedLines{t_tabCapLines}, tabs{tabs_t} {}


CommandLineLayout::CommandLineLayout(const Modes mode_t, const strView currFileName_t, const strView t_currCommand, const int cursorX_t, const int cursorY_t,
    const int charCount_t, const int lineCount_t, const strView commandLineArgs_t)
    :
    mode{mode_t}, currentFileName{currFileName_t}, currentCommand{t_currCommand}, cursorX{cursorX_t}, cursorY{cursorY_t}, charCount{charCount_t},
    lineCount{lineCount_t}, commandLineArgs{commandLineArgs_t} {}

PanesLayout::PanesLayout(const int t_startX, const int t_startY, const int t_endX, const int t_endY,
    const int t_leftDataOffsetX, const std::vector<strView>& t_leftData, const std::vector<strView>& t_lines)
    :
    startX{t_startX}, startY{t_startY}, endX{t_endX}, endY{t_endY}, leftDataOffsetX{t_leftDataOffsetX}, leftData{t_leftData}, lines{t_lines} {}

CursorLayout::CursorLayout(const bool t_visible, const int t_cursorX, const int t_cursorY, const char t_letter, const PanesLayout& t_panesLayout)
    : visible{t_visible}, cursorX{t_cursorX}, cursorY{t_cursorY}, letter{t_letter}, panesLayout{(&t_panesLayout)} {}

LayoutManager::LayoutManager(const int t_windowWidth, const int t_windowHeight): windowWidth{t_windowWidth}, windowHeight{t_windowHeight} {}

void LayoutManager::addTabLayout(const TabLayout& t_tabLayout) {
    tabLayout = t_tabLayout;
}

void LayoutManager::addPanesLayout(const PanesLayout& t_panesLayout) {
    panesLayout.push_back(t_panesLayout);
}

void LayoutManager::addCursorLayout(const CursorLayout& t_cursorLayout) {
    cursorLayout = t_cursorLayout;
}

void LayoutManager::addCommandLineLayout(const CommandLineLayout& t_commandLineLayout) {
    commandLineLayout = t_commandLineLayout;
}
