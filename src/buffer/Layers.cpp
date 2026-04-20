#include "Layers.hpp"

#include <format>

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

	const auto filterSpecialFiles = [&files](const File& t_file) { return files.regularFile(t_file.fileId_); };

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
