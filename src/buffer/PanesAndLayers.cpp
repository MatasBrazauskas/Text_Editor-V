#include "PanesAndLayers.hpp"

#include "core/EditorCore.hpp"
#include "utils/ConfigAndSettings.hpp"

#include <algorithm>
#include <cmath>
#include <format>
#include <ranges>

Coordinates::Coordinates(int sx, int sy, int ex, int ey) : startX{sx}, startY{sy}, endX{ex}, endY{ey} {}

TextIndex::TextIndex(const int t_indexX, const int t_indexY) : indexX{t_indexX}, indexY{t_indexY} {}

Cursor::Cursor() : x_{}, y_{}, visible_{true}, absent_{} {}

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
	absent_ = 4;
}

void Cursor::setY(const int y) {
	y_ = y;
	visible_ = true;
	absent_ = 4;
}

bool Cursor::isVisible() const {
	return visible_;
}

void Cursor::setVisible(const bool visible) {
	visible_ = visible;
}

Pane::Pane(const PaneId t_paneId, const FileId t_fileId)
	: paneId_{t_paneId}, fileId_{t_fileId}, textIndex_{0, 0}, cursor_{} {}

SplitNode::SplitNode(const Pane t_pane) : nodeType{t_pane} {}

SplitNode::SplitNode(SplitType t_splitType) : nodeType{t_splitType} {}

PanesManager::PanesManager() : activePaneId_{}, head_{nullptr} {}

void PanesManager::addPane(const PaneId t_parentId, const FileId t_fileId, const AddedPaneRotation t_rotation) {
	const auto pane = Pane{paneIdCounter_++, t_fileId};

	if (head_ == nullptr) {
		head_ = new SplitNode(pane);
	} else {
		auto parentOption = getPanePointer(t_parentId);

		if (parentOption != std::nullopt) {
			auto parent = parentOption.value();
			const Pane parentPane = get<Pane>(parent->nodeType);

			Pane leftChildPane = parentPane;
			Pane rightChildPane = parentPane;
			SplitType splitType;

			switch (t_rotation) {
			case AddedPaneRotation::Right:
				leftChildPane = parentPane;
				rightChildPane = pane;
				splitType = SplitType::Vertical;
				break;
			case AddedPaneRotation::Left:
				leftChildPane = pane;
				rightChildPane = parentPane;
				splitType = SplitType::Vertical;
				break;
			case AddedPaneRotation::Top:
				leftChildPane = pane;
				rightChildPane = parentPane;
				splitType = SplitType::Horizontal;
				break;
			case AddedPaneRotation::Bottom:
				leftChildPane = parentPane;
				rightChildPane = pane;
				splitType = SplitType::Horizontal;
				break;
			}

			parent->nodeType = splitType;

			parent->leftChild = std::make_unique<SplitNode>(leftChildPane);
			parent->rightChild = std::make_unique<SplitNode>(rightChildPane);

			return;
		}

		throw std::runtime_error{"No split node found"};
	}
}

void PanesManager::removePane(const PaneId t_paneId) {
	const auto parentOpt = getPaneParentPointer(t_paneId);

	if (parentOpt == std::nullopt) {
		throw std::runtime_error{"No split node found, child or parent"};
	}

	const auto parent = parentOpt.value();
	const Pane leftChildPane = get<Pane>(parent->leftChild->nodeType);
	const Pane rightChildPane = get<Pane>(parent->rightChild->nodeType);

	const Pane anotherChildPane = leftChildPane.paneId_ == t_paneId ? rightChildPane : leftChildPane;

	if (rightChildPane.paneId_ == t_paneId) {
		parent->nodeType = anotherChildPane;
	} else if (leftChildPane.paneId_ == t_paneId) {
		parent->nodeType = anotherChildPane;
	} else {
		throw std::runtime_error("Id doesnt match the children of a parent");
	}

	parent->leftChild.reset();
	parent->rightChild.reset();
}

std::optional<SplitNode*> PanesManager::getPaneParentPointer(const PaneId t_paneId) {
	const auto splitNodes = getAllSplitNode();

	const auto filterInternalNodes = [&](const SplitNode* splitNode) {
		const bool internalNodePredicate = std::holds_alternative<SplitType>(splitNode->nodeType);
		if (internalNodePredicate == false) {
			return false;
		}

		const bool parentToLeafPredicate = std::holds_alternative<Pane>(splitNode->leftChild->nodeType) &&
										   std::holds_alternative<Pane>(splitNode->rightChild->nodeType);
		return parentToLeafPredicate;
	};

	const auto findLeafParent = [&](const SplitNode* internalNode) {
		const auto leftId = get<Pane>(internalNode->leftChild->nodeType).paneId_;
		const auto rightId = get<Pane>(internalNode->rightChild->nodeType).paneId_;

		return leftId == t_paneId || rightId == t_paneId;
	};

	auto internalNodes = splitNodes | std::ranges::views::filter(filterInternalNodes);
	std::vector<SplitNode*> tempVec{internalNodes.begin(), internalNodes.end()};

	const auto it = std::ranges::find_if(internalNodes, findLeafParent);

	if (it != internalNodes.end()) {
		return *it;
	}

	return std::nullopt;
}

static void addCoordinates(std::vector<PaneInfo>& t_coordinates, SplitNode* t_splitNode, Coordinates t_cords) {
	if (t_splitNode != nullptr) {
		if (std::holds_alternative<SplitType>(t_splitNode->nodeType)) {
			const auto split = get<SplitType>(t_splitNode->nodeType);

			const auto leftChild = t_splitNode->leftChild.get();
			const auto rightChild = t_splitNode->rightChild.get();

			if (split == SplitType::Vertical) {
				Coordinates leftCoords = t_cords;
				Coordinates rightCoords = t_cords;

				const int widthDiff = t_cords.endX - t_cords.startX;
				const int leftDiff =
					static_cast<int>(std::round(static_cast<float>(widthDiff) * t_splitNode->leftChildRation));
				const int rightDiff = widthDiff - leftDiff;

				leftCoords.endX = leftCoords.startX + leftDiff;
				rightCoords.startX = rightDiff;

				addCoordinates(t_coordinates, leftChild, leftCoords);
				addCoordinates(t_coordinates, rightChild, rightCoords);
			} else if (split == SplitType::Horizontal) {
				Coordinates topCoords = t_cords;
				Coordinates bottomCoords = t_cords;

				const int heightDiff = t_cords.endY - t_cords.startY;
				const int topDiff =
					static_cast<int>(std::round(static_cast<float>(heightDiff) * t_splitNode->leftChildRation));
				const int bottomDiff = heightDiff - topDiff;

				topCoords.endY = topCoords.startY + topDiff;
				bottomCoords.startY = bottomDiff;

				addCoordinates(t_coordinates, leftChild, topCoords);
				addCoordinates(t_coordinates, rightChild, bottomCoords);
			}
		} else if (std::holds_alternative<Pane>(t_splitNode->nodeType)) {
			const auto pane = get<Pane>(t_splitNode->nodeType);
			t_coordinates.emplace_back(pane.fileId_, pane.textIndex_, pane.cursor_, t_cords);
		}
	}
}

std::vector<PaneInfo> PanesManager::getPaneCoordinates(const int t_height, const int t_width) const {
	std::vector<PaneInfo> coordinates;
	addCoordinates(coordinates, head_, {0, 0, t_width, t_height});
	return coordinates;
}

std::optional<Pane> PanesManager::getPane(const PaneId t_paneId) {
	if (const auto pane = getPanePointer(t_paneId); pane != nullptr) {
		return get<Pane>(pane.value()->nodeType);
	}
	return std::nullopt;
}

std::optional<Pane> PanesManager::getCurrPane() {
	return getPane(this->activePaneId_);
}

static void addPaneToList(std::vector<SplitNode*>& list, SplitNode* splitNode) {
	if (splitNode != nullptr) {
		list.push_back(splitNode);

		addPaneToList(list, splitNode->leftChild.get());
		addPaneToList(list, splitNode->rightChild.get());
	}
}

std::vector<SplitNode*> PanesManager::getAllSplitNode() {
	std::vector<SplitNode*> splitNodes;

	addPaneToList(splitNodes, head_);

	return splitNodes;
}

void PanesManager::shiftPane(const PaneId t_paneId, const PaneSizeChange t_change) {
	const auto parentOpt = getPaneParentPointer(t_paneId);

	if (parentOpt == std::nullopt) {
		throw std::runtime_error("Pane id is invalid");
	}

	const auto parent = parentOpt.value();
	const auto left = get<Pane>(parent->leftChild.get()->nodeType);
	const auto right = get<Pane>(parent->rightChild.get()->nodeType);

	if (left.paneId_ == t_paneId) {
		switch (t_change) {
		case PaneSizeChange::Contract:
			parent->leftChildRation = std::clamp(parent->leftChildRation - 0.1f, 0.1f, 0.9f);
			break;
		case PaneSizeChange::Expand:
			parent->leftChildRation = std::clamp(parent->leftChildRation + 0.1f, 0.1f, 0.9f);
			break;
		}

	} else if (right.paneId_ == t_paneId) {
		switch (t_change) {
		case PaneSizeChange::Contract:
			parent->leftChildRation = std::clamp(parent->leftChildRation + 0.1f, 0.1f, 0.9f);
			break;
		case PaneSizeChange::Expand:
			parent->leftChildRation = std::clamp(parent->leftChildRation - 0.1f, 0.1f, 0.9f);
			break;
		}
	} else {
		throw std::runtime_error("Pane id is invalid");
	}
}

void PanesManager::resetRatios() {
	const auto splitNodes = getAllSplitNode();

	const auto filterInternalNodes = [&](const SplitNode* splitNode) {
		const bool internalNodePredicate = std::holds_alternative<SplitType>(splitNode->nodeType);
		if (internalNodePredicate == false) {
			return false;
		}

		const bool parentToLeafPredicate = std::holds_alternative<Pane>(splitNode->leftChild->nodeType) &&
										   std::holds_alternative<Pane>(splitNode->rightChild->nodeType);
		return parentToLeafPredicate;
	};
	auto internalNodes = splitNodes | std::ranges::views::filter(filterInternalNodes);

	for (auto& intNode : internalNodes) {
		intNode->leftChildRation = 0.5f;
	}
}

std::optional<SplitNode*> PanesManager::getPanePointer(const PaneId t_paneId) {
	const auto temp = [&](const SplitNode* splitNode) {
		if (std::holds_alternative<Pane>(splitNode->nodeType)) {
			const auto paneId = get<Pane>(splitNode->nodeType).paneId_;
			return paneId == t_paneId;
		}
		return false;
	};

	const auto splitNodes = getAllSplitNode();
	const auto it = std::ranges::find_if(splitNodes, temp);

	if (it != splitNodes.end()) {
		return *it;
	}

	return std::nullopt;
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

LayoutManager::LayoutManager(EditorCore& t_editorCore, const Config& t_config, const Settings& t_settings)
	: windowHeight{t_config.window.height}, windowWidth{t_config.window.width} {
	auto& fileManager = t_editorCore.getFilesManager();
	auto& paneManager = t_editorCore.getPanesManager();
	const auto& editorState = t_editorCore.getEditorState();
	const auto& editorIO = t_editorCore.getEditorInputAndOutput();

	addTabLayout(fileManager, t_settings);
	addPanesLayout(fileManager, paneManager, t_settings, tabLayout.tabCapturedLinesOffsetY, t_config);
	addCursorLayout(paneManager, t_settings, fileManager);
	addCommandLineLayout(paneManager, t_settings, editorState, editorIO, fileManager);
}

void LayoutManager::addTabLayout(const FilesManager& files, const Settings& t_settings) {
	const auto& [codeCharWidth, codeCharHeight, uiCharWidth, uiCharHeight, tabHeight] = t_settings.charSettings;

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

	int lineCount = 1;
	int currentX = 0;
	int paddingX = uiCharWidth * 2;

	for (const auto& tab : tabVec) {
		int tabWidth = (tab.length() * uiCharWidth) + paddingX;

		// Check if this tab forces a wrap to the next line
		if (currentX + tabWidth > windowWidth) {
			lineCount++;
			currentX = tabWidth; // Reset X to the start of the new line
		} else {
			currentX += tabWidth;
		}
	}

	/*const auto tempLambda = [&](const int a, std::string_view b) {
		const int tabWidth = (b.length() * uiCharWidth) + (uiCharWidth * 2);
		if (a + tabWidth) {
			return 0;
		}
		return tabWidth;
	};
	const int tabLines = std::accumulate(tabVec.begin(), tabVec.end(), 1, tempLambda);*/

	int activePane{};
	const auto it = std::ranges::find(fileVec, 0, &File::fileId_);
	if (it != fileVec.end()) {
		activePane = std::distance(fileVec.begin(), it);
	}

	this->tabLayout.activeTab = activePane;
	this->tabLayout.tabCapturedLinesOffsetY = lineCount * tabHeight;
	this->tabLayout.tabs = tabVec;
}

static std::vector<std::string> leftSideNumbers(int startIndex, int endIndex, int cursorY, LineNumberModes lineMode, const File& t_file, const Config& t_config) {
	std::vector<std::string> leftSide;

	const auto addNumber = [](const int lineNumber) {
		return std::format("{:>4}", lineNumber);
	};
	const auto addRelativeNumber = [](const int lineNumber, const int cursorY) {
		const int lineIndex = lineNumber == cursorY ? cursorY : std::abs(lineNumber - cursorY);
		return std::format("{:>4}", lineIndex);
	};

	for (auto it = t_file.textBuffer_.forwardIterator(startIndex); !it.end(endIndex); it.next()){
		std::string number;
		if (lineMode == LineNumberModes::Number) {
			number = addNumber(it.index_);
		}
		else if (lineMode == LineNumberModes::Relative) {
			number = addRelativeNumber(it.index_, cursorY);
		}

		if (t_config.editor.view.lineInfo) {
			char symb = ' ';

			if (t_file.textBuffer_.lineInfo_[it.index_] == LineInfo::Insert) {
				symb = '+';
			} else if (t_file.textBuffer_.lineInfo_[it.index_] == LineInfo::Changed) {
				symb = '~';
			}

			number.insert(0,1,symb);
		}

		leftSide.push_back(number);
	}

	return leftSide;
}

void LayoutManager::addPanesLayout(FilesManager& t_filesManager, const PanesManager& t_panesManager,
								   const Settings& t_settings, const int t_tabOffsetY, const Config& t_config) {
	const int w = t_settings.windowSettings.width;
	const int h = t_settings.windowSettings.height - t_tabOffsetY - 2 * t_settings.charSettings.uiCharHeight;

	const auto panesInfo = t_panesManager.getPaneCoordinates(h, w);

	for (const auto& [fileId, textIndex, cursor, coords] : panesInfo) {
		const auto& file = t_filesManager.getFile(fileId);

		const int charWidthCount = std::floor((coords.endX - coords.startX) / t_settings.charSettings.codeCharWidth);
		const int charHeightCount = std::floor((coords.endY - coords.startY) / t_settings.charSettings.codeCharHeight);

		const int startIndexX = std::floor(coords.startX / t_settings.charSettings.codeCharWidth);
		const int startIndexY = std::floor(coords.startY / t_settings.charSettings.codeCharWidth);

		const int endIndexY = std::min(startIndexY + charHeightCount, file.textBuffer_.getLinesCount());

		std::vector<std::string> linesVec;
		linesVec.reserve(endIndexY - startIndexY);
		std::vector<std::string> leftSide = leftSideNumbers(startIndexY, endIndexY, cursor.getY(), t_config.editor.view.lineNumberMode, file, t_config);

		const int endIndexX = startIndexX + charWidthCount - leftSide.at(0).length();

		for (auto it = file.textBuffer_.forwardIterator(startIndexY); !it.end(endIndexY); it.next()) {
			const std::string_view strView = it.getLine();

			if (strView.length() < startIndexX) {
				linesVec.push_back("");
				continue;
			}

			const int length = std::min(endIndexX, static_cast<int>(strView.length()));
			std::string_view subStrView = strView.substr(startIndexX, length - startIndexX);

			linesVec.emplace_back(subStrView);
		}

		const auto& layoutPane = PanesLayout(coords.startX, t_tabOffsetY + coords.startY, coords.endX,
											 t_tabOffsetY + coords.endY, leftSide.at(0).length() * t_settings.charSettings.codeCharWidth, leftSide, linesVec);
		panesLayout.push_back(layoutPane);
	}
}

void LayoutManager::addCursorLayout(PanesManager& t_paneManager, const Settings& t_settings,
									FilesManager& t_filesManager) {
	/*const auto pane = t_paneManager.paneMap_[t_paneManager.activePaneId_];
	const auto [startX, startY, endX, endY] = pane->paneView_;
	const auto& [codeCharWidth, codeCharHeight, uiCharWidth, uiCharHeight, tabHeight] = t_settings.charSettings;

	const int cursorOffsetX = (pane->cursor_.getY() - indexY) * uiCharWidth + startX;
	const int cursorOffsetY = (pane->cursor_.getY() - indexX) * uiCharHeight + startY;

	const auto line = t_filesManager.getFile(pane->fileId_).textBuffer_.getLine(pane->cursor_.getY());
	char cursorChar = ' ';

	if (line.length() < pane->cursor_.getX()) {
		cursorChar = line.at(pane->cursor_.getX());
	}

	cursorLayout = CursorLayout(pane->cursor_.isVisible(), cursorChar, cursorOffsetX, cursorOffsetY,
	CursorType::Block);*/
}

void LayoutManager::addCommandLineLayout(PanesManager& t_panesManager, const Settings& t_constConfig,
										 const EditorState& t_editorState, const EditorInputAndOutput& t_io,
										 FilesManager& t_filesManager) {
	const Modes mode = t_editorState.currentMode_;
	const std::string fileName = t_filesManager.getFile().filesPath_.filename();
	const std::string currCommand = t_io.input_;

	const auto activePane = t_panesManager.getCurrPane();
	const int cursorPotionX = activePane->cursor_.getX();
	const int cursorPotionY = activePane->cursor_.getY();

	const int charCount = t_filesManager.getFile().textBuffer_.getCharCount();
	const int linesCount = t_filesManager.getFile().textBuffer_.getLinesCount();

	const std::string commandLineArgs = t_io.commandLineMessage_;

	commandLineLayout = CommandLineLayout(mode, fileName, currCommand, cursorPotionX, cursorPotionY, charCount,
										  linesCount, commandLineArgs);
}
