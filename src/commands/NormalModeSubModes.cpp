#include "NormalModeSubModes.hpp"

#include "core/EditorCore.hpp"
#include "utils/ConfigAndSettings.hpp"

WindowSubCommand::WindowSubCommand() {
	functionMap_ = {{'v', &WindowSubCommand::verticalSplit}, {'s', &WindowSubCommand::horizontalSplit},
					{'h', &WindowSubCommand::movePaneLeft},	 {'j', &WindowSubCommand::moveToPaneDown},
					{'k', &WindowSubCommand::movePaneUp},	 {'l', &WindowSubCommand::movePaneRight},
					{'c', &WindowSubCommand::closePane},	 {'=', &WindowSubCommand::equalizePanes}};
}
void WindowSubCommand::ExecuteCommand(PanesManager& t_panesManager, WindowSettings& t_winSettings,
									  const char t_inputChar) const {

	const auto it = functionMap_.find(t_inputChar);
	if (it != functionMap_.end()) {
		(this->*it->second)(t_panesManager, t_winSettings);
	}
}

void WindowSubCommand::verticalSplit(PanesManager& t_panesManager, WindowSettings&) const {
	const auto& pane = t_panesManager.getCurrPane();
	t_panesManager.addPane(pane->paneId_, pane->fileId_, PaneDirection::Left);
}

void WindowSubCommand::horizontalSplit(PanesManager& t_panesManager, WindowSettings&) const {
	const auto& pane = t_panesManager.getCurrPane();
	t_panesManager.addPane(pane->paneId_, pane->fileId_, PaneDirection::Bottom);
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
	const auto paneId = t_panesManager.getCurrPane()->paneId_;
	t_panesManager.removePane(paneId);
}

void WindowSubCommand::equalizePanes(PanesManager& t_panesManager, WindowSettings&) const {
	t_panesManager.resetRatios();
}

FileSubCommand::FileSubCommand() {
	functionMap_ = {
		{'s', &FileSubCommand::openInHorizontal},
		{'v', &FileSubCommand::openInVertical},
		{'k', &FileSubCommand::moveUp},
		{'j', &FileSubCommand::moveDown},
		/*{static_cast<char>(SpecialKeys::Enter), &FileSubCommand::open},
		{'r', &FileSubCommand::refresh},*/
	};
}

std::vector<std::string> getDirectoriesContent() {
	std::vector dirContent = {"../"s};

	for (const auto& file : std::filesystem::directory_iterator{"."}) {
		const auto filePath = file.path().string();
		const auto index = filePath.find_last_of('/');
		auto fileName = std::string(filePath.substr(index + 1));

		if (file.is_directory()) {
			fileName.push_back('/');
		}
		dirContent.push_back(fileName);
	}

	return dirContent;
}

void FileSubCommand::ExecuteCommand(PanesManager& t_panesManager, FilesManager& t_filesManager,
									WindowSettings& t_winSettings, const char t_inputChar) const {
	const auto it = functionMap_.find(t_inputChar);
	if (it != functionMap_.end()) {
		(this->*it->second)(t_panesManager, t_filesManager, t_winSettings);
	}
}

void FileSubCommand::openInVertical(PanesManager& t_panesManager, FilesManager& t_filesManager,
									WindowSettings& t_winSettings) const {
	const auto dirContent = getDirectoriesContent();
	const auto matrix = Matrix(dirContent);
	const auto fileId = t_filesManager.addSpecialFile(matrix);

	const auto& pane = t_panesManager.getCurrPane();
	t_panesManager.addPane(pane->paneId_, fileId, PaneDirection::Left);
}

void FileSubCommand::openInHorizontal(PanesManager& t_panesManager, FilesManager& t_filesManager,
									  WindowSettings& t_winSettings) const {
	const auto dirContent = getDirectoriesContent();
	const auto matrix = Matrix(dirContent);
	const auto fileId = t_filesManager.addSpecialFile(matrix);

	const auto& pane = t_panesManager.getCurrPane();
	t_panesManager.addPane(pane->paneId_, fileId, PaneDirection::Bottom);
}

void FileSubCommand::moveUp(PanesManager& t_panesManager, FilesManager&, WindowSettings&) const {
	auto cursor = t_panesManager.getCurrPane()->cursor_;
	cursor.decrementY();
}

void FileSubCommand::moveDown(PanesManager& t_panesManager, FilesManager&, WindowSettings&) const {
	auto cursor = t_panesManager.getCurrPane()->cursor_;
	cursor.incrementY();
}