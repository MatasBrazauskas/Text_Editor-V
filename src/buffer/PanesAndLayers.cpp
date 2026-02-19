#include "PanesAndLayers.hpp"

#include <algorithm>
#include <ranges>

Pane::Pane(const PaneView paneView_t, const BufferView textView_t, const FileId t_fileId, const PaneId t_paneId)
    : paneView_{paneView_t}, textView_{textView_t}, fileId_{t_fileId}, paneId_{t_paneId} {}

Pane::Pane(const Pane& t_pane): Pane(t_pane.paneView_, t_pane.textView_, t_pane.fileId_, t_pane.paneId_) {}

SplitNode::SplitNode(const SplitType t_splitType, const bool t_isLeaf, const Pane& t_pane): splitType{t_splitType}, isLeaf {t_isLeaf} {
    pane = std::make_unique<Pane>(t_pane.paneView_, t_pane.textView_, t_pane.fileId_, t_pane.paneId_);
}

PanesManager::PanesManager(const Pane& t_pane): activePaneId_{}, head_{SplitType::None, false, t_pane} {
    paneMap_.insert(std::pair{paneIdCounter_, head_.pane.get()});
    paneIdCounter_++;
}

Pane PanesManager::getPane(const PaneId t_paneId) {
    const auto pane = paneMap_[t_paneId];
    return Pane{*pane};
}

TabLayout::TabLayout(const int activeTab_t, const int t_tabCapLines,  const std::vector<std::string>& tabs_t)
    : activeTab{activeTab_t}, tabCapturedLinesOffsetY{t_tabCapLines}, tabs{tabs_t} {}


CommandLineLayout::CommandLineLayout(const Modes mode_t, const std::string currFileName_t, const std::string t_currCommand, const int cursorX_t, const int cursorY_t,
    const int charCount_t, const int lineCount_t, const std::string commandLineArgs_t)
    :
    mode{mode_t}, currentFileName{currFileName_t}, currentCommand{t_currCommand}, cursorX{cursorX_t}, cursorY{cursorY_t}, charCount{charCount_t},
    lineCount{lineCount_t}, commandLineArgs{commandLineArgs_t} {}

PanesLayout::PanesLayout(const int t_startX, const int t_startY, const int t_endX, const int t_endY,
    const int t_leftDataOffsetX, const std::vector<std::string>& t_leftData, const std::vector<std::string>& t_lines)
    :
    startX{t_startX}, startY{t_startY}, endX{t_endX}, endY{t_endY}, leftDataOffsetX{t_leftDataOffsetX}, leftData{t_leftData}, lines{t_lines} {}

CursorLayout::CursorLayout(const bool t_visible, const int t_cursorX, const int t_cursorY, const char t_letter, const PaneId t_paneId)
    : visible{t_visible}, cursorX{t_cursorX}, cursorY{t_cursorY}, letter{t_letter}, paneId{t_paneId} {}

LayoutManager::LayoutManager(const EditorCore& t_editorCore, const Config& t_config) {
    const auto& files = t_editorCore.getFiles();
    const auto& paneManager = t_editorCore.getPanesManager();
    const auto& editorState = t_editorCore.getEditorState();
    const auto& editorIO = t_editorCore.getEditorInputAndOutput();

    addTabLayout(files, t_config.constantConfig_);
    addPanesLayout(files, paneManager, t_config);
}

void LayoutManager::addTabLayout(const FilesManager& files, const ConstantsConfig& constConfig) {
    std::vector<std::string_view> tabVec;

    auto to_filename_view = [](const auto& obj) {
        std::string_view sv = obj.filesPath_.native();
        const auto pos = sv.find_last_of("/\\");
        return pos == std::string_view::npos ? sv : sv.substr(pos + 1);
    };

    std::ranges::transform(files.files_, std::back_inserter(tabVec), to_filename_view);

    const auto temp = [constConfig](int a, std::string_view b) {
        const int tabWidth = (b.length() * constConfig.uiCharWidth) + (constConfig.uiCharWidth * 2);
        if (a + tabWidth) {
            return 0;
        }
        return tabWidth;
    };
    const int tabLines = std::accumulate(tabVec.begin(), tabVec.end(), 0, temp);

    int activePane{};
    const auto it = std::ranges::find(files.files_, 0, &File::fileId_);
    if (it != files.files_.end()) {
        activePane = std::distance(files.files_.begin(), it);
    }

    this->tabLayout.activeTab = activePane;
    this->tabLayout.tabCapturedLinesOffsetY = tabLines * constConfig.tabHeight;
    this->tabLayout.tabs = tabVec;
}

void LayoutManager::addPanesLayout(FilesManager& t_filesManager, const PanesManager& t_panesLayout, const ConstantsConfig& t_config) {
    const auto& pane = t_panesLayout.head_.pane.get();
    const auto file = t_filesManager.getFile(pane->fileId_);
    const int charCountInWidth = (pane->paneView_.endX_ - pane->paneView_.startX) / t_config.codeCharWidth;
    const int charCountInHeight = (pane->paneView_.endY_ - pane->paneView_.startY) / t_config.codeCharHeight;

    std::vector<std::string> linesVec;

    for (const auto it = file.get().textBuffer_->forwardIterator(charCountInHeight); !it->end(); it->next()) {
        std::string_view strView = it->getLine();

        if (strView.length() < (pane->paneView_.startX / t_config.codeCharWidth)) {
            linesVec.push_back("");
            continue;
        }

        int length = std::min(pane->paneView_.endX_ / t_config.codeCharWidth + charCountInWidth, static_cast<int>(strView.length()));
        std::string_view subStrView = strView.substr((pane->paneView_.startX / t_config.codeCharWidth, length);
        linesVec.push_back(subStrView.data());
    }

    std::vector<std::string> leftSide;
    const auto& layoutPane = PanesLayout(pane->paneView_.startX, pane->paneView_.endX_, pane->paneView_.startY, pane->paneView_.endY_, 0, leftSide, linesVec);
    panesLayout.push_back(layoutPane);
}

void LayoutManager::addCursorLayout(const PanesManager& t_paneManager) {
    const auto* pane = t_paneManager.paneMap_[t_paneManager.activePaneId_];

}

void LayoutManager::addCommandLineLayout(const CommandLineLayout& t_commandLineLayout) {
    commandLineLayout = t_commandLineLayout;
}