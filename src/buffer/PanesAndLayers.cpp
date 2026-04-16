#include "PanesAndLayers.hpp"

#include "core/EditorCore.hpp"
#include "utils/ConfigAndSettings.hpp"

#include <algorithm>
#include <cmath>
#include <format>
#include <ranges>

Coordinates::Coordinates(int sx, int sy, int ex, int ey) : startX{sx}, startY{sy}, endX{ex}, endY{ey} {}

TextIndex::TextIndex(const int t_indexX, const int t_indexY) : indexX{t_indexX}, indexY{t_indexY} {}

Cursor::Cursor() : absent_{}, x_{}, y_{}, visible_{true} {}

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

Pane::Pane(const PaneId t_paneId, const FileId t_fileId) : paneId_{t_paneId}, fileId_{t_fileId}, textIndex_{0, 0}, cursors_{} {
	cursors_.insert({fileId_, Cursor()});
}

Cursor& Pane::getCursor() {
	return cursors_[fileId_];
}

void Pane::switchFileId(const FileId t_fileId) {
	if (cursors_.contains(fileId_)) {
		cursors_.erase(fileId_);
	}

	cursors_.insert({t_fileId, Cursor()});
	fileId_ = t_fileId;
}

SplitNode::SplitNode(const Pane t_pane) : nodeType{t_pane} {}

SplitNode::SplitNode(SplitType t_splitType) : nodeType{t_splitType} {}

PanesManager::PanesManager() : activePaneId_{}, paneHistoryManager_{}, head_{nullptr} {}

void PanesManager::addPane(const PaneId t_parentId, const FileId t_fileId, const PaneDirection t_rotation) {
	const auto pane = Pane{paneIdCounter_++, t_fileId};

	if (head_ == nullptr) {
		head_ = new SplitNode(pane);
		paneHistoryManager_.addPane(pane.paneId_);
	} else {
		auto parentOption = getPanePointer(t_parentId);

		if (parentOption != std::nullopt) {
			auto parent = parentOption.value();
			const Pane parentPane = get<Pane>(parent->nodeType);

			Pane leftChildPane = parentPane;
			Pane rightChildPane = parentPane;
			SplitType splitType;

			switch (t_rotation) {
			case PaneDirection::Right:
				leftChildPane = parentPane;
				rightChildPane = pane;
				splitType = SplitType::Vertical;
				break;
			case PaneDirection::Left:
				leftChildPane = pane;
				rightChildPane = parentPane;
				splitType = SplitType::Vertical;
				break;
			case PaneDirection::Top:
				leftChildPane = pane;
				rightChildPane = parentPane;
				splitType = SplitType::Horizontal;
				break;
			case PaneDirection::Bottom:
				leftChildPane = parentPane;
				rightChildPane = pane;
				splitType = SplitType::Horizontal;
				break;
			}

			parent->nodeType = splitType;

			parent->leftChild = std::make_unique<SplitNode>(leftChildPane);
			parent->rightChild = std::make_unique<SplitNode>(rightChildPane);

			if (paneHistoryManager_.containsPane(pane.paneId_)) {
				paneHistoryManager_.pushUpPane(pane.paneId_);
			} else {
				paneHistoryManager_.addPane(pane.paneId_);
			}

			return;
		}

		throw std::runtime_error{"No split node found"};
	}
}

void PanesManager::removePane(const PaneId t_paneId) {
	const auto parentOpt = getPaneParentPointer(t_paneId);

	if (parentOpt == std::nullopt) {
		return;
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

	paneHistoryManager_.removePane(t_paneId);
	activePaneId_ = paneHistoryManager_.historyArr.at(paneHistoryManager_.historySize - 1);
}

std::optional<SplitNode*> PanesManager::getPaneParentPointer(const PaneId t_paneId) {
	const auto splitNodes = getAllSplitNode();

	const auto filterInternalNodes = [&](const SplitNode* splitNode) {
		const bool internalNodePredicate = std::holds_alternative<SplitType>(splitNode->nodeType);
		if (internalNodePredicate == false) {
			return false;
		}

		const bool parentToLeafPredicate =
			std::holds_alternative<Pane>(splitNode->leftChild->nodeType) && std::holds_alternative<Pane>(splitNode->rightChild->nodeType);
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
				const int leftDiff = static_cast<int>(std::round(static_cast<float>(widthDiff) * t_splitNode->leftChildRation));

				leftCoords.endX = leftCoords.startX + leftDiff;
				rightCoords.startX = leftCoords.endX;

				addCoordinates(t_coordinates, leftChild, leftCoords);
				addCoordinates(t_coordinates, rightChild, rightCoords);
			} else if (split == SplitType::Horizontal) {
				Coordinates topCoords = t_cords;
				Coordinates bottomCoords = t_cords;

				const int heightDiff = t_cords.endY - t_cords.startY;
				const int topDiff = static_cast<int>(std::round(static_cast<float>(heightDiff) * t_splitNode->leftChildRation));

				topCoords.endY = topCoords.startY + topDiff;
				bottomCoords.startY = topCoords.endY;

				addCoordinates(t_coordinates, leftChild, topCoords);
				addCoordinates(t_coordinates, rightChild, bottomCoords);
			}
		} else if (std::holds_alternative<Pane>(t_splitNode->nodeType)) {
			const auto pane = get<Pane>(t_splitNode->nodeType);
			t_coordinates.emplace_back(pane.paneId_, pane.fileId_, pane.textIndex_, pane.cursors_.at(pane.fileId_), t_cords);
		}
	}
}

std::vector<PaneInfo> PanesManager::getPaneCoordinates(const int t_height, const int t_width) const {
	std::vector<PaneInfo> coordinates;
	addCoordinates(coordinates, head_, {0, 0, t_width, t_height});
	return coordinates;
}

Pane& PanesManager::getPane(const PaneId t_paneId) {
	const auto pane = getPanePointer(t_paneId);
	return get<Pane>(pane.value()->nodeType);
}

Pane& PanesManager::getCurrPane() {
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

PaneHistoryManager::PaneHistoryManager() : historyArr{}, historySize{} {}

bool PaneHistoryManager::containsPane(const PaneId t_paneId) const {
	return std::find(historyArr.begin(), historyArr.end(), t_paneId) != historyArr.end();
}

void PaneHistoryManager::addPane(const PaneId t_paneId) {
	if (historySize >= historyArr.size()) {
		std::shift_left(historyArr.begin(), historyArr.end(), 1);
		historySize--;
	}

	historyArr[historySize++] = t_paneId;
}

void PaneHistoryManager::pushUpPane(const PaneId t_paneId) {
	removePane(t_paneId);
	historyArr[historySize++] = t_paneId;
}

void PaneHistoryManager::removePane(const PaneId t_paneId) {
	const auto it = std::find(historyArr.begin(), historyArr.begin() + historySize, t_paneId);

	if (it != historyArr.begin() + historySize) {
		std::shift_left(it, historyArr.begin() + historySize, 1);
		historyArr.at(historySize - 1) = 0;
		historySize--;
	}
}

PaneId PaneHistoryManager::getLastPaneId() const {
	return historyArr.at(historySize - 1);
}

void PanesManager::moveToPane(const int t_height, const int t_width, PaneDirection t_paneDirection) {
	const auto topPredicate = [](const Coordinates& currPaneCoords, const Coordinates& paneCoords) {
		const bool heightPredicate = currPaneCoords.startY == paneCoords.endY;
		const bool endsOutside = paneCoords.startX <= currPaneCoords.startX && currPaneCoords.endX <= paneCoords.endX;
		const bool endsInside = (currPaneCoords.startX <= paneCoords.startX && paneCoords.startX <= currPaneCoords.endX) ||
								(currPaneCoords.startX <= paneCoords.endX && paneCoords.endX <= currPaneCoords.endX);

		return heightPredicate && (endsOutside || endsInside);
	};

	const auto bottomPredicate = [](const Coordinates& currPaneCoords, const Coordinates& paneCoords) {
		const bool heightPredicate = currPaneCoords.endY == paneCoords.startY;
		const bool endsOutside = paneCoords.startX <= currPaneCoords.startX && currPaneCoords.endX <= paneCoords.endX;
		const bool endsInside = (currPaneCoords.startX <= paneCoords.startX && paneCoords.startX <= currPaneCoords.endX) ||
								(currPaneCoords.startX <= paneCoords.endX && paneCoords.endX <= currPaneCoords.endX);

		return heightPredicate && (endsOutside || endsInside);
	};

	const auto leftPredicate = [](const Coordinates& currPaneCoords, const Coordinates& paneCoords) {
		const bool heightPredicate = currPaneCoords.startX == paneCoords.endX;
		const bool endsOutside = paneCoords.startY <= currPaneCoords.startY && currPaneCoords.endY <= paneCoords.endY;
		const bool endsInside = (currPaneCoords.startY <= paneCoords.startY && paneCoords.startY <= currPaneCoords.endY) ||
								(currPaneCoords.startY <= paneCoords.endY && paneCoords.endY <= currPaneCoords.endY);

		return heightPredicate && (endsOutside || endsInside);
	};

	const auto rightPredicate = [](const Coordinates& currPaneCoords, const Coordinates& paneCoords) {
		const bool heightPredicate = currPaneCoords.endX == paneCoords.startX;
		const bool endsOutside = paneCoords.startY <= currPaneCoords.startY && currPaneCoords.endY <= paneCoords.endY;
		const bool endsInside = (currPaneCoords.startY <= paneCoords.startY && paneCoords.startY <= currPaneCoords.endY) ||
								(currPaneCoords.startY <= paneCoords.endY && paneCoords.endY <= currPaneCoords.endY);

		return heightPredicate && (endsOutside || endsInside);
	};

	const auto panesCoords = getPaneCoordinates(t_height, t_width);

	const auto paneIdPredicate = [&](const PaneInfo& t_pane) {
		const auto& paneId = std::get<0>(t_pane);
		return paneId == activePaneId_;
	};
	const auto it = std::ranges::find_if(panesCoords, paneIdPredicate);
	const auto& currPaneCoords = std::get<4>(*it);

	std::vector<PaneId> borderingPanes;

	for (const auto& paneInfo : panesCoords) {
		const auto& paneCoords = std::get<4>(paneInfo);

		bool flag{};

		switch (t_paneDirection) {
		case PaneDirection::Top:
			flag = topPredicate(currPaneCoords, paneCoords);
			break;
		case PaneDirection::Bottom:
			flag = bottomPredicate(currPaneCoords, paneCoords);
			break;
		case PaneDirection::Left:
			flag = leftPredicate(currPaneCoords, paneCoords);
			break;
		case PaneDirection::Right:
			flag = rightPredicate(currPaneCoords, paneCoords);
			break;
		}

		if (flag) {
			borderingPanes.push_back(std::get<0>(paneInfo));
		}
	}

	for (const auto& cachedPane : paneHistoryManager_) {
		for (auto i = borderingPanes.rbegin(); i != borderingPanes.rend(); ++i) {
			if (*i == cachedPane) {
				activePaneId_ = cachedPane;

				if (paneHistoryManager_.containsPane(activePaneId_)) {
					paneHistoryManager_.pushUpPane(activePaneId_);
				} else {
					paneHistoryManager_.addPane(activePaneId_);
				}

				return;
			}
		}
	}
}

void PanesManager::shiftPane(const PaneId t_paneId, const PaneSizeChange t_change) {
	/*const auto parentOpt = getPaneParentPointer(t_paneId);

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
	}*/
}

void PanesManager::resetRatios() {
	const auto splitNodes = getAllSplitNode();

	for (auto& node : splitNodes) {
		const bool internalNodePredicate = std::holds_alternative<SplitType>(node->nodeType);
		if (internalNodePredicate) {
			node->leftChildRation = 0.5f;
		}
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

CommandLineLayout::CommandLineLayout(Modes t_mode, std::string t_modeName, std::string t_inputInfo, std::string t_fileInto,
									 CommandLineState t_state, int t_cursorX, std::string t)
	: mode{t_mode}, modeName{t_modeName}, inputInfo{t_inputInfo}, fileInfo{t_fileInto}, commandLineState{t_state}, cursorIndexX{t_cursorX},
	  commandLineInfo{t} {}

PanesLayout::PanesLayout(const PanesSnippets t_panesSnippet, const int t_startX, const int t_startY, const int t_endX, const int t_endY,
						 const int t_leftDataOffsetX, const std::vector<std::string>& t_leftData, const std::vector<std::string>& t_lines)
	: panesSnippet{t_panesSnippet}, startX{t_startX}, startY{t_startY}, endX{t_endX}, endY{t_endY}, leftDataOffsetX{t_leftDataOffsetX},
	  leftData{t_leftData}, lines{t_lines} {}

CursorLayout::CursorLayout(const bool t_visible, const int t_cursorX, const int t_cursorY, const std::string t_letters,
						   const int t_cursorWidth, const CursorType t_cursorType)
	: visible{t_visible}, cursorX{t_cursorX}, cursorY{t_cursorY}, letter{t_letters}, cursorWidth{t_cursorWidth}, cursorType{t_cursorType} {}

LayoutManager::LayoutManager(EditorCore& t_editorCore, const Config& t_config, const Settings& t_settings)
	: windowHeight{t_config.window.height}, windowWidth{t_config.window.width} {
	auto& fileManager = t_editorCore.filesManager_;
	auto& paneManager = t_editorCore.panesManager_;
	const auto& editorState = t_editorCore.editorState_;
	const auto& editorIO = t_editorCore.editorInputAndOutput_;

	addTabLayout(fileManager, t_settings);
	int left = 0;
	addPanesLayout(fileManager, paneManager, t_settings, tabLayout.tabCapturedLinesOffsetY, t_config, left);
	addCursorLayout(paneManager, t_settings, fileManager, tabLayout.tabCapturedLinesOffsetY, left, editorState);
	addCommandLineLayout(paneManager, t_settings, editorState, editorIO, fileManager);
}

void LayoutManager::addTabLayout(const FilesManager& files, const Settings& t_settings) {
	const auto& [codeCharWidth, codeCharHeight, uiCharWidth, uiCharHeight, tabHeight] = t_settings.charSettings;

	auto to_filename_view = [](const File& t_file) -> std::string {
		auto sv = std::string{t_file.filesPath_.filename()};
		if (t_file.textBuffer_.dirty) {
			sv.insert(0, 1, '*');
		}
		return sv;
	};

	const auto filterSpecialFiles = [&files](const File& t_file) {
		return files.regularFile(t_file.fileId_);
	};

	auto temp = files.files_ | std::views::filter(filterSpecialFiles) | std::views::transform(to_filename_view);

	std::vector<std::string> tabVec{temp.begin(), temp.end()};

	int lineCount = 1;
	int currentX = 0;
	int paddingX = uiCharWidth * 2;

	for (const auto& tab : tabVec) {
		const int tabWidth = (tab.length() * uiCharWidth) + paddingX;

		if (currentX + tabWidth > windowWidth) {
			lineCount++;
			currentX = tabWidth;
		} else {
			currentX += tabWidth;
		}
	}

	int activePane{};
	const auto it = std::ranges::find(files.files_, files.activeFileId_, &File::fileId_);
	if (it != files.files_.end()) {
		activePane = std::distance(files.files_.begin(), it);
	}

	this->tabLayout.activeTab = activePane;
	this->tabLayout.tabCapturedLinesOffsetY = lineCount * tabHeight;
	this->tabLayout.tabs = tabVec;
}

static std::vector<std::string> leftSideNumbers(int startIndex, int endIndex, int cursorY, LineNumberModes lineMode, const File& t_file,
												const Config& t_config) {
	std::vector<std::string> leftSide;

	const auto addNumber = [](const int lineNumber) { return std::format("{:^5}", lineNumber); };
	const auto addRelativeNumber = [](const int lineNumber, const int cursorY) {
		if (lineNumber == cursorY) {
			return std::format("{:^4}", cursorY) + " ";
		}
		const int lineIndex = std::abs(lineNumber - cursorY);
		return std::format("{:^5}", lineIndex);
	};

	for (auto it = t_file.textBuffer_.forwardIterator(startIndex); !it.end(endIndex); it.next()) {
		std::string number;
		if (lineMode == LineNumberModes::Number) {
			number = addNumber(it.index_);
		} else if (lineMode == LineNumberModes::Relative) {
			number = addRelativeNumber(it.index_, cursorY);
		}

		if (t_config.editor.view.lineNumberMode != LineNumberModes::None) {
			char symb = ' ';

			if (t_file.textBuffer_.lineInfo_[it.index_] == LineInfo::Insert) {
				symb = '+';
			} else if (t_file.textBuffer_.lineInfo_[it.index_] == LineInfo::Changed) {
				symb = '~';
			}

			number.insert(0, 1, symb);
		}

		leftSide.push_back(number);
	}

	return leftSide;
}

static std::string expandTabs(const std::string_view line, const int tabWidth) {
	std::string result;
	result.reserve(line.size());

	int visualCol = 0;

	for (char c : line) {
		if (c == '\t') {
			int spaces = tabWidth - (visualCol % tabWidth);
			result.append(spaces, ' ');
			visualCol += spaces;
		} else {
			result.push_back(c);
			++visualCol;
		}
	}

	return result;
}

void LayoutManager::addPanesLayout(FilesManager& t_filesManager, const PanesManager& t_panesManager, const Settings& t_settings,
								   const int t_tabOffsetY, const Config& t_config, int& t_left) {
	const int w = t_settings.windowSettings.width;
	const int h = t_settings.windowSettings.height - t_tabOffsetY - 2 * t_settings.charSettings.uiCharHeight;

	const auto panesInfo = t_panesManager.getPaneCoordinates(h, w);

	for (const auto& [paneId, fileId, textIndex, cursor, coords] : panesInfo) {
		const auto& file = t_filesManager.getFile(fileId);

		const int charWidthCount = std::floor((coords.endX - coords.startX) / t_settings.charSettings.codeCharWidth);
		const int charHeightCount = std::floor((coords.endY - coords.startY) / t_settings.charSettings.codeCharHeight);

		const int startIndexX = textIndex.indexX;
		const int startIndexY = textIndex.indexY;

		const int endIndexY = std::min(startIndexY + charHeightCount, file.textBuffer_.getLinesCount());

		std::vector<std::string> linesVec;
		linesVec.reserve(endIndexY - startIndexY);
		std::vector<std::string> leftSide =
			leftSideNumbers(startIndexY, endIndexY, cursor.getY(), t_config.editor.view.lineNumberMode, file, t_config);

		const int endIndexX = leftSide.empty() ? 0 : startIndexX + charWidthCount - leftSide.at(0).length();

		for (auto it = file.textBuffer_.forwardIterator(startIndexY); !it.end(endIndexY); it.next()) {
			const auto line = expandTabs(it.getLine(), t_config.editor.feel.indentSize);
			const auto strView = std::string_view{line};

			if (strView.length() < startIndexX) {
				linesVec.push_back("");
				continue;
			}

			const int length = std::min(endIndexX, static_cast<int>(strView.length()));
			std::string_view subStrView = strView.substr(startIndexX, length - startIndexX);

			linesVec.emplace_back(subStrView);
		}

		int leftSideOffset = leftSide.empty() ? 0 : leftSide.at(0).length();
		t_left = leftSideOffset * t_settings.charSettings.codeCharWidth;
		PanesSnippets panesSnippets = PanesSnippets::TextSnippet;

		if (t_filesManager.specialFile(file.fileId_)) {
			leftSideOffset = 0;
			leftSide.clear();
			t_left = 0;
			panesSnippets = PanesSnippets::FilesSnippet;
		}

		const auto& layoutPane =
			PanesLayout(panesSnippets, coords.startX, t_tabOffsetY + coords.startY, coords.endX, t_tabOffsetY + coords.endY,
						leftSideOffset * t_settings.charSettings.codeCharWidth, leftSide, linesVec);
		panesLayout.push_back(layoutPane);
	}
}

void LayoutManager::addCursorLayout(PanesManager& t_panesManager, const Settings& t_settings, FilesManager& t_filesManager,
									int t_tabOffsetY, int t_leftSideOffsetX, const EditorState& t_editorState) {
	const int w = t_settings.windowSettings.width;
	const int h = t_settings.windowSettings.height - t_tabOffsetY - 2 * t_settings.charSettings.uiCharHeight;

	const auto panesInfo = t_panesManager.getPaneCoordinates(h, w);
	const auto& [paneId, fileId, textIndex, cursor, coords] = *std::ranges::find_if(
		panesInfo, [&](const PaneInfo& t_paneInfo) { return std::get<0>(t_paneInfo) == t_panesManager.activePaneId_; });

	const auto& cur = t_panesManager.getPane(paneId).getCursor();

	const auto& file = t_filesManager.getFile(fileId);

	if (t_filesManager.specialFile(fileId)) {
		t_leftSideOffsetX = 0;
	}

	const int cursorX = coords.startX + t_leftSideOffsetX + cursor.getX() * t_settings.charSettings.codeCharWidth;
	const int cursorY = coords.startY + t_tabOffsetY + cursor.getY() * t_settings.charSettings.codeCharHeight;

	auto letters = std::string{" "};
	if (auto line = file.textBuffer_.getLine(cursor.getY()); line.length() != 0 && cursor.getX() < line.length())
		letters = std::string{file.textBuffer_.getLine(cursor.getY()).at(cursor.getX())};
	auto cursorType = CursorType::Block;
	int cursorWidth = t_settings.charSettings.codeCharWidth;

	if (t_editorState.currentMode_ == Modes::Insert) {
		cursorType = CursorType::Stick;
		cursorWidth = 1;
		letters = "";
	} else if (t_editorState.currentMode_ == Modes::FileMode) {
		cursorType = CursorType::Line;
		letters = file.textBuffer_.getLine(cursor.getY());
		cursorWidth = letters.length() * t_settings.charSettings.codeCharWidth;
	}

	cursorLayout = CursorLayout{cur.isVisible(), cursorX, cursorY, letters, cursorWidth, cursorType};
}

void LayoutManager::addCommandLineLayout(PanesManager&, const Settings&, const EditorState& t_editorState, const EditorInputAndOutput& t_io,
										 FilesManager& t_filesManager) {
	const static std::unordered_map<Modes, std::string> modeNameMap = {{Modes::Normal, "  Normal"},
																	   {Modes::Insert, "  Insert"},
																	   {Modes::Command, "  Command"},
																	   {Modes::WindowMode, "  Window"},
																	   {Modes::FileMode, "   Files"}};

	const Modes mode = t_editorState.currentMode_;
	const auto modeName = modeNameMap.at(mode);

	const auto inputInfo = std::format("{:<}", t_io.commandLineMessage_);

	const int charCount = t_filesManager.getFile().textBuffer_.getCharCount();
	const int lineCount = t_filesManager.getFile().textBuffer_.getLinesCount();
	const auto fileInfo = std::format("Lines: {}, Chars: {}", lineCount, charCount);

	const auto commandLineInfo = t_io.commandLineMessage_;

	commandLineLayout = CommandLineLayout(mode, modeName, inputInfo, fileInfo, t_io.commandLineState_, t_io.cursorIndexX, commandLineInfo);
}
