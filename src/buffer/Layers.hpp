#pragma once

#include "core/EditorCore.hpp"
#include "utils/ConfigAndSettings.hpp"

#include <string>
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
	CommandLineLayout(Modes mode_t, std::string, std::string, std::string, CommandLineState t_state, int t_cursorX, std::string);
	~CommandLineLayout() noexcept = default;

	Modes mode;
	std::string modeName;

	std::string inputInfo;
	std::string fileInfo;

	CommandLineState commandLineState;
	int cursorIndexX;
	std::string commandLineInfo;
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
	void addPanesLayout(FilesManager&, const PanesManager&, const Settings&, int t_tabOffsetY, const Config&, int& t_left);
	void addCursorLayout(PanesManager& t_paneManager, const Settings& t_config, FilesManager& t_filesManager, int t_tabOffsetY,
						 int t_leftSideOffsetX, const EditorState&);
	void addCommandLineLayout(PanesManager& t_panesManager, const Settings& t_constConfig, const EditorState& t_editorState,
							  const EditorInputAndOutput& t_io, FilesManager& t_filesManager);
};