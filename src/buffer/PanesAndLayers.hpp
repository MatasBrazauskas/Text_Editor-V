#pragma once

#include <array>
#include <memory>
#include <optional>
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
using PaneId = uint_fast64_t;

enum class Modes : uint8_t;
enum class NormalModeModes : char;

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

  private:
	int x_;
	int y_;
	bool visible_;
	int absent_;
};

class Pane final {
  public:
	Pane() = delete;
	Pane(PaneId, FileId);
	~Pane() noexcept = default;

	PaneId paneId_;
	FileId fileId_;
	TextIndex textIndex_;
	Cursor cursor_;
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

	void addPaneId(PaneId);
	void removePaneId(PaneId);
	std::optional<PaneId> getLastPaneId();

	auto begin() const {
		return std::make_reverse_iterator(historyArr.begin() + historySize);
	}
	auto end() const {
		return std::make_reverse_iterator(historyArr.begin());
	}

  private:
	std::array<PaneId, 8> historyArr;
	int historySize;
};

class PanesManager final {
  public:
	PanesManager();
	~PanesManager() noexcept = default;

	std::optional<std::reference_wrapper<Pane>> getPane(PaneId);
	std::optional<std::reference_wrapper<Pane>> getCurrPane();

	void addPane(PaneId t_parent, FileId t_fileId, PaneDirection t_rotation);
	void removePane(PaneId);

	void moveToPane(int t_height, int t_width, PaneDirection);

	void shiftPane(PaneId, PaneSizeChange);
	void resetRatios();

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

class TabLayout final {
  public:
	TabLayout() = default;
	TabLayout(int activeTab_t, int t_tabCapLines, const std::vector<std::string>& tabs_t);
	~TabLayout() noexcept = default;

	int activeTab;
	int tabCapturedLinesOffsetY;
	std::vector<std::string> tabs;
};

class CommandLineLayout final {
  public:
	CommandLineLayout() = default;
	CommandLineLayout(Modes mode_t, std::string, std::string, std::string, std::string);
	~CommandLineLayout() noexcept = default;

	Modes mode;

	std::string modeName;
	std::string commandInfo;
	std::string fileInfo;
	std::string commandLineArgs;
};

enum class PanesSnippets { TextSnippet, FilesSnippet };

class PanesLayout final {
  public:
	PanesLayout() = default;
	PanesLayout(PanesSnippets t_panesSnippet, int t_startX, int t_startY, int t_endX, int t_endY, int t_leftDataOffsetX,
				const std::vector<std::string>& t_leftData, const std::vector<std::string>& t_lines);
	~PanesLayout() noexcept = default;

	PanesSnippets panesSnippet;

	int startX, startY;
	int endX, endY;
	int leftDataOffsetX;

	std::vector<std::string> leftData;
	std::vector<std::string> lines;
};

enum class CursorType { Stick, Block, Line };

class CursorLayout final {
  public:
	CursorLayout() = default;
	CursorLayout(bool t_visible, int t_cursorX, int t_cursorY, std::string t_letters, int t_cursorWidth, CursorType);
	~CursorLayout() noexcept = default;

	bool visible;
	int cursorX, cursorY;
	std::string letter;
	int cursorWidth;
	CursorType cursorType;
};

class LayoutManager final {
  public:
	LayoutManager() = delete;
	explicit LayoutManager(EditorCore&, const Config&, const Settings&);
	~LayoutManager() noexcept = default;

	int windowHeight, windowWidth;

	TabLayout tabLayout;
	std::vector<PanesLayout> panesLayout;
	CursorLayout cursorLayout;
	CommandLineLayout commandLineLayout;

  private:
	void addTabLayout(const FilesManager&, const Settings&);
	void addPanesLayout(FilesManager&, const PanesManager&, const Settings&, int t_tabOffsetY, const Config&,
						int& t_left);
	void addCursorLayout(PanesManager& t_paneManager, const Settings& t_config, FilesManager& t_filesManager,
						 int t_tabOffsetY, int t_leftSideOffsetX, const EditorState&);
	void addCommandLineLayout(PanesManager& t_panesManager, const Settings& t_constConfig,
							  const EditorState& t_editorState, const EditorInputAndOutput& t_io,
							  FilesManager& t_filesManager);
};