#pragma once

#include <array>
#include <memory>
#include <optional>
#include <unordered_map>
#include <variant>
#include <vector>

using FileId = std::uint_fast64_t;
using PaneId = uint_fast64_t;

class EditorCore;
class EditorState;
class EditorInputAndOutput;
class Settings;
class Config;
class FilesManager;

using FileId = std::uint_fast64_t;
using PaneId = std::uint_fast64_t;

enum class Modes : uint8_t;
enum class NormalModeModes : char;
enum class CommandLineState : char;

enum class SplitType : char { Vertical, Horizontal };
enum class PaneDirection : char { Top, Bottom, Right, Left };
enum class PaneSizeChange : char { Expand, Contract };

class Coordinates {
  public:
	Coordinates() = delete;
	Coordinates(int, int, int, int);
	~Coordinates() noexcept = default;

	int startX, startY, endX, endY;
};

class TextIndex {
  public:
	TextIndex() = delete;
	TextIndex(int, int);
	~TextIndex() noexcept = default;

	int indexX, indexY;
};

class Cursor final {
  public:
	Cursor();
	~Cursor() noexcept = default;

	void incrementX();
	void decrementX();
	void incrementY();
	void decrementY();

	[[nodiscard]] int getX() const;
	[[nodiscard]] int getY() const;

	void setX(int);
	void setY(int);

	[[nodiscard]] bool isVisible() const;
	void setVisible(bool);

	int absent_;

  private:
	int x_;
	int y_;
	bool visible_;
};

class Pane final {
  public:
	Pane() = delete;
	Pane(PaneId, FileId);
	~Pane() noexcept = default;

	Cursor& getCursor();
	void switchFileId(FileId);

	PaneId paneId_;
	FileId fileId_;
	TextIndex textIndex_;
	std::unordered_map<FileId, Cursor> cursors_;
};

class SplitNode final {
  public:
	explicit SplitNode(SplitType);
	explicit SplitNode(Pane);
	~SplitNode() noexcept = default;

	float leftChildRation = 0.5f;

	std::variant<Pane, SplitType> nodeType;

	std::unique_ptr<SplitNode> leftChild;
	std::unique_ptr<SplitNode> rightChild;
};

using PaneInfo = std::tuple<PaneId, FileId, TextIndex, Cursor, Coordinates>;

class PaneHistoryManager {
  public:
	PaneHistoryManager();
	~PaneHistoryManager() noexcept = default;

	bool containsPane(PaneId) const;
	void addPane(PaneId);
	void pushUpPane(PaneId);
	void removePane(PaneId);

	PaneId getLastPaneId() const;

	auto begin() const {
		return std::make_reverse_iterator(historyArr.begin() + historySize);
	}
	auto end() const {
		return std::make_reverse_iterator(historyArr.begin());
	}

	std::array<PaneId, 8> historyArr;
	int historySize;
};

class PanesManager final {
  public:
	PanesManager();
	~PanesManager() noexcept = default;

	Pane& getPane(PaneId);
	Pane& getCurrPane();

	void addPane(PaneId t_parent, FileId t_fileId, PaneDirection t_rotation);
	void removePane(PaneId);

	void moveToPane(int t_height, int t_width, PaneDirection);

	void shiftPane(PaneId, PaneSizeChange);
	void resetRatios();

	void addSpecialPane(FileId);
	void removeSpecialPane();
	void setActivePaneToSpecialPane();

	std::vector<PaneInfo> getPaneCoordinates(int t_height, int t_width) const;
	PaneId activePaneId_;

  private:
	inline static PaneId paneIdCounter_{};
	PaneHistoryManager paneHistoryManager_;

	SplitNode* head_;

	std::vector<SplitNode*> getAllSplitNode();
	std::optional<SplitNode*> getPanePointer(PaneId);
	std::optional<SplitNode*> getPaneParentPointer(PaneId);
};