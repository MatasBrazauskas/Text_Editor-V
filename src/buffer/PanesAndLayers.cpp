#include "PanesAndLayers.hpp"

#include "core/EditorCore.hpp"
#include "utils/Config.hpp"

#include <algorithm>
#include <ranges>

Cursor::Cursor() : x_{0}, y_{0}, visible_{true}, absent_{} {}

void Cursor::incrementX() {
	this->setX(x_ + 1);
}

void Cursor::decrementX() {
	this->setX(x_ - 1);
}

void Cursor::incrementY() {
	this->setY(y_ + 1);
}

void Cursor::decrementY() {
	this->setY(y_ - 1);
}

int Cursor::getX() const {
	return x_;
}

int Cursor::getY() const {
	return y_;
}

void Cursor::setX(const int x) {
	x_ = x;
	visible_ = true;
	absent_ = framesToSkip;
}

void Cursor::setY(const int y) {
	y_ = y;
	visible_ = true;
	absent_ = framesToSkip;
}

bool Cursor::isVisible() const {
	return visible_;
}

void Cursor::setVisible(const bool visible) {
	visible_ = visible;
}

Pane::Pane(const PaneView paneView_t, const FileId t_fileId, const PaneId t_paneId)
	: paneView_{paneView_t}, fileId_{t_fileId}, paneId_{t_paneId} {}

Pane::Pane(const Pane& t_pane) : Pane(t_pane.paneView_, t_pane.fileId_, t_pane.paneId_) {}

SplitNode::SplitNode(const SplitType t_splitType, const bool t_isLeaf, const Pane& t_pane)
	: splitType{t_splitType}, isLeaf{t_isLeaf} {
	pane = std::make_unique<Pane>(t_pane.paneView_, t_pane.fileId_, t_pane.paneId_);
}

PanesManager::PanesManager(const PaneView& t_view, FileId t_fileId)
	: activePaneId_{}, head_{SplitType::None, false, Pane(t_view, paneIdCounter_, t_fileId)} {
	paneMap_.insert(std::pair{paneIdCounter_, head_.pane.get()});
	paneIdCounter_++;
}

Pane PanesManager::getPane(const PaneId t_paneId) {
	const auto pane = paneMap_[t_paneId];
	return Pane{*pane};
}

Pane PanesManager::getCurrPane() {
	return getPane(paneIdCounter_);
}

TabLayout::TabLayout(const int activeTab_t, const int t_tabCapLines, const std::vector<std::string>& tabs_t)
	: activeTab{activeTab_t}, tabCapturedLinesOffsetY{t_tabCapLines}, tabs{tabs_t} {}

CommandLineLayout::CommandLineLayout(const Modes mode_t, const std::string currFileName_t,
									 const std::string t_currCommand, const int cursorX_t, const int cursorY_t,
									 const int charCount_t, const int lineCount_t, const std::string commandLineArgs_t)
	: mode{mode_t}, currentFileName{currFileName_t}, currentCommand{t_currCommand}, cursorX{cursorX_t},
	  cursorY{cursorY_t}, charCount{charCount_t}, lineCount{lineCount_t}, commandLineArgs{commandLineArgs_t} {}

PanesLayout::PanesLayout(const int t_startX, const int t_startY, const int t_endX, const int t_endY,
						 const int t_leftDataOffsetX, const std::vector<std::string>& t_leftData,
						 const std::vector<std::string>& t_lines)
	: startX{t_startX}, startY{t_startY}, endX{t_endX}, endY{t_endY}, leftDataOffsetX{t_leftDataOffsetX},
	  leftData{t_leftData}, lines{t_lines} {}

CursorLayout::CursorLayout(const bool t_visible, const int t_cursorX, const int t_cursorY, const char t_letter,
						   const CursorType t_cursorType)
	: visible{t_visible}, cursorX{t_cursorX}, cursorY{t_cursorY}, letter{t_letter}, cursorType{t_cursorType} {}

LayoutManager::LayoutManager(EditorCore& t_editorCore, const Config& t_config) : windowHeight{800}, windowWidth{1000} {
	auto& fileManager = t_editorCore.getFilesManager();
	auto& paneManager = t_editorCore.getPanesManager();
	const auto& editorState = t_editorCore.getEditorState();
	const auto& editorIO = t_editorCore.getEditorInputAndOutput();

	addTabLayout(fileManager, t_config.constantConfig_);
	addPanesLayout(fileManager, paneManager, t_config.constantConfig_, tabLayout.tabCapturedLinesOffsetY);
	addCursorLayout(paneManager, t_config.constantConfig_, fileManager);
	addCommandLineLayout(paneManager, t_config.constantConfig_, t_editorCore.getEditorState(),
						 t_editorCore.getEditorInputAndOutput(), fileManager);
}

void LayoutManager::addTabLayout(const FilesManager& files, const ConstantsConfig& constConfig) {

	auto to_filename_view = [](const File& t_file) -> std::string {
		std::string_view sv = t_file.filesPath_.native();
		const auto pos = sv.find_last_of("/\\");

		if (pos != std::string::npos) {
			sv = sv.substr(pos + 1);
		}

		return std::string{sv};
	};

	const auto fileVec = files.files_ | std::views::values;
	const auto temp = fileVec | std::views::transform(to_filename_view);

	const std::vector<std::string> tabVec{temp.begin(), temp.end()};

	const auto tempLambda = [constConfig](int a, std::string_view b) {
		const int tabWidth = (b.length() * constConfig.uiCharWidth) + (constConfig.uiCharWidth * 2);
		if (a + tabWidth) {
			return 0;
		}
		return tabWidth;
	};
	const int tabLines = std::accumulate(tabVec.begin(), tabVec.end(), 0, tempLambda);

	int activePane{};
	const auto it = std::ranges::find(fileVec, 0, &File::fileId_);
	if (it != fileVec.end()) {
		activePane = std::distance(fileVec.begin(), it);
	}

	this->tabLayout.activeTab = activePane;
	this->tabLayout.tabCapturedLinesOffsetY = tabLines * constConfig.tabHeight;
	this->tabLayout.tabs = tabVec;
}

void LayoutManager::addPanesLayout(FilesManager& t_filesManager, const PanesManager& t_panesLayout,
								   const ConstantsConfig& t_config, const int t_tabOffsetY) {
	const auto& pane = t_panesLayout.head_.pane.get();
	const auto file = t_filesManager.getFile(pane->fileId_);
	const auto& [text, stack, path, id] = file;

	const int startIndexY = pane->paneView_.startY / t_config.codeCharHeight;
	const int startIndexX = pane->paneView_.startX / t_config.codeCharHeight;
	const int charCountInWidth = (pane->paneView_.endX_ - pane->paneView_.startX) / t_config.codeCharWidth;
	const int charCountInHeight = (pane->paneView_.endY_ - pane->paneView_.startY) / t_config.codeCharHeight;

	std::vector<std::string> linesVec;
	std::vector<std::string> leftSide;

	for (auto it = text.forwardIterator(startIndexY); !it.end(startIndexY + charCountInHeight); it.next()) {
		std::string_view strView = it.getLine();

		if (strView.length() < startIndexX) {
			linesVec.push_back("");
			continue;
		}

		const int length = std::min(startIndexX + charCountInWidth, static_cast<int>(strView.length()));
		std::string_view subStrView = strView.substr(startIndexX, length);
		linesVec.push_back(subStrView.data());
	}

	const auto& layoutPane =
		PanesLayout(pane->paneView_.startX, pane->paneView_.endX_, pane->paneView_.startY + t_tabOffsetY,
					pane->paneView_.endY_, 0, leftSide, linesVec);
	panesLayout.push_back(layoutPane);
}

void LayoutManager::addCursorLayout(PanesManager& t_paneManager, const ConstantsConfig& t_config,
									FilesManager& t_filesManager) {
	const auto pane = t_paneManager.paneMap_[t_paneManager.activePaneId_];
	const auto [startX, startY, endX_, endY_, indexX, indexY] = pane->paneView_;

	const int cursorOffsetX = (pane->cursor_.getY() - indexY) * t_config.uiCharWidth + startX;
	const int cursorOffsetY = (pane->cursor_.getY() - indexX) * t_config.uiCharHeight + startY;

	const auto line = t_filesManager.getFile(pane->fileId_).textBuffer_.getLine(pane->cursor_.getY());
	char cursorChar = ' ';

	if (line.length() < pane->cursor_.getX()) {
		cursorChar = line.at(pane->cursor_.getX());
	}

	cursorLayout = CursorLayout(pane->cursor_.isVisible(), cursorChar, cursorOffsetX, cursorOffsetY, CursorType::Block);
}

void LayoutManager::addCommandLineLayout(PanesManager& t_panesManager, const ConstantsConfig& t_constConfig,
										 const EditorState& t_editorState, const EditorInputAndOutput& t_io,
										 FilesManager& t_filesManager) {
	const Modes mode = t_editorState.currentMode_;
	const std::string fileName = t_filesManager.getFile().filesPath_.filename();
	const std::string currCommand = t_io.input_;

	const int cursorPotionX = t_panesManager.paneMap_[t_panesManager.activePaneId_]->cursor_.getX();
	const int cursorPotionY = t_panesManager.paneMap_[t_panesManager.activePaneId_]->cursor_.getY();

	const int charCount = t_filesManager.getFile().textBuffer_.getCharCount();
	const int linesCount = t_filesManager.getFile().textBuffer_.getLinesCount();

	const std::string commandLineArgs = t_io.commandLineMessage_;

	commandLineLayout = CommandLineLayout(mode, fileName, currCommand, cursorPotionX, cursorPotionY, charCount,
										  linesCount, commandLineArgs);
}
