#include "NormalModeSubModes.hpp"

#include "core/EditorCore.hpp"
#include "utils/ConfigAndSettings.hpp"

WindowSubCommand::WindowSubCommand() {
	functionMap_ = {{'v', &WindowSubCommand::verticalSplit}, {'s', &WindowSubCommand::horizontalSplit},
					{'h', &WindowSubCommand::movePaneLeft},	 {'j', &WindowSubCommand::moveToPaneDown},
					{'k', &WindowSubCommand::movePaneUp},	 {'l', &WindowSubCommand::movePaneRight},
					{'c', &WindowSubCommand::closePane},	 {'=', &WindowSubCommand::equalizePanes}};
}
void WindowSubCommand::ExecuteCommand(PanesManager& t_panesManager, WindowSettings& t_winSettings, const char t_inputChar) const {

	const auto it = functionMap_.find(t_inputChar);
	if (it != functionMap_.end()) {
		(this->*it->second)(t_panesManager, t_winSettings);
	}
}

void WindowSubCommand::verticalSplit(PanesManager& t_panesManager, WindowSettings&) const {
	const auto& pane = t_panesManager.getCurrPane();
	t_panesManager.addPane(pane.paneId_, pane.fileId_, PaneDirection::Left);
}

void WindowSubCommand::horizontalSplit(PanesManager& t_panesManager, WindowSettings&) const {
	const auto& pane = t_panesManager.getCurrPane();
	t_panesManager.addPane(pane.paneId_, pane.fileId_, PaneDirection::Bottom);
}

void WindowSubCommand::movePaneLeft(PanesManager& t_panesManager, WindowSettings& t_winSettings) const {
	t_panesManager.moveToPane(t_winSettings.height, t_winSettings.width, PaneDirection::Left);
}

void WindowSubCommand::movePaneRight(PanesManager& t_panesManager, WindowSettings& t_winSettings) const {
	t_panesManager.moveToPane(t_winSettings.height, t_winSettings.width, PaneDirection::Right);
}

void WindowSubCommand::moveToPaneDown(PanesManager& t_panesManager, WindowSettings& t_winSettings) const {
	t_panesManager.moveToPane(t_winSettings.height, t_winSettings.width, PaneDirection::Bottom);
}

void WindowSubCommand::movePaneUp(PanesManager& t_panesManager, WindowSettings& t_winSettings) const {
	t_panesManager.moveToPane(t_winSettings.height, t_winSettings.width, PaneDirection::Top);
}

void WindowSubCommand::closePane(PanesManager& t_panesManager, WindowSettings&) const {
	const auto paneId = t_panesManager.getCurrPane().paneId_;
	t_panesManager.removePane(paneId);
}

void WindowSubCommand::equalizePanes(PanesManager& t_panesManager, WindowSettings&) const {
	t_panesManager.resetRatios();
}

FileSubCommand::FileSubCommand() {
	functionMap_ = {
		{'k', &FileSubCommand::moveUp},
		{'j', &FileSubCommand::moveDown},
		{'e', &FileSubCommand::open},
		{'c', &FileSubCommand::close},
		{'h', &FileSubCommand::moveUpParentDir},
	};
}

void FileSubCommand::ExecuteCommand(PanesManager& t_panesManager, FilesManager& t_filesManager, WindowSettings& t_winSettings,
									const char t_inputChar) const {
	const auto it = functionMap_.find(t_inputChar);
	if (it != functionMap_.end()) {
		(this->*it->second)(t_panesManager, t_filesManager, t_winSettings);
	}
}

void FileSubCommand::openFileMode(PanesManager& t_panesManager, FilesManager& t_filesManager) const {
	if (t_filesManager.specialFile_ == std::nullopt) {
		const auto fileId = t_filesManager.addSpecialFile();
		t_filesManager.activeFileId_ = fileId;
		t_panesManager.addSpecialPane(fileId);
	} else {
		t_filesManager.activeFileId_ = t_filesManager.specialFile_.value();
		t_panesManager.setActivePaneToSpecialPane();
	}
}

void FileSubCommand::moveUp(PanesManager& t_panesManager, FilesManager&, WindowSettings&) const {
	auto& cursor = t_panesManager.getCurrPane().getCursor();
	cursor.decrementY();
}

void FileSubCommand::moveDown(PanesManager& t_panesManager, FilesManager&, WindowSettings&) const {
	auto& cursor = t_panesManager.getCurrPane().getCursor();
	cursor.incrementY();
}

void FileSubCommand::open(PanesManager& t_panesManager, FilesManager& t_filesManager, WindowSettings&) const {
	auto& pane = t_panesManager.getCurrPane();
	const auto& cursor = pane.getCursor();
	const auto& file = t_filesManager.getFile(pane.fileId_);

	const auto filename = file.textBuffer_.getLine(cursor.getY());
	const auto filePath = std::filesystem::path{file.filesPath_.string() + std::string{filename}};

	if (std::filesystem::is_directory(filePath)) {
		t_filesManager.changeSpecialFile(file.fileId_, filePath);
		pane.cursors_.at(file.fileId_) = Cursor();
	} else if (std::filesystem::is_regular_file(filePath)) {
		const auto fileId = t_filesManager.addRegularFile(std::filesystem::path(filePath));

		pane.switchFileId(fileId);
		t_filesManager.activeFileId_ = fileId;
	}
}

void FileSubCommand::close(PanesManager& t_panesManager, FilesManager& t_filesManager, WindowSettings&) const {
	t_panesManager.removeSpecialPane();
	t_filesManager.removeFile(t_filesManager.activeFileId_);
}

void FileSubCommand::moveUpParentDir(PanesManager& t_panesManager, FilesManager& t_filesManager, WindowSettings&) const {
	auto& pane = t_panesManager.getCurrPane();
	const auto filePath = t_filesManager.getFile().filesPath_ / "..";
	t_filesManager.changeSpecialFile(t_filesManager.activeFileId_, filePath);
	pane.cursors_.at(t_filesManager.activeFileId_) = Cursor();
}