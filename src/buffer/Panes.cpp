#include "Panes.hpp"

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

			activePaneId_ = pane.paneId_;

			return;
		}

		throw std::runtime_error{"No split node found"};
	}
}

void PanesManager::addSpecialPane(const FileId t_fileId) {
	const auto fileModePane = Pane(paneIdCounter_++, t_fileId);
	auto* newHead = new SplitNode(SplitType::Vertical);
	newHead->leftChildRation = 0.25f;

	newHead->rightChild = std::unique_ptr<SplitNode>(head_);
	newHead->leftChild = std::make_unique<SplitNode>(fileModePane);

	head_ = newHead;
	activePaneId_ = fileModePane.paneId_;
}

void PanesManager::setActivePaneToSpecialPane() {
	const auto& fileModePane = get<Pane>(head_->leftChild->nodeType);
	activePaneId_ = fileModePane.paneId_;
}

void PanesManager::removeSpecialPane() {
	activePaneId_ = paneHistoryManager_.getLastPaneId();
	head_ = head_->rightChild.get();
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