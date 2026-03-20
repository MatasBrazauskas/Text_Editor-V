#include "NormalModeSubModes.hpp"

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
		{'k', &FileSubCommand::moveUp},
		{'j', &FileSubCommand::moveDown},
		{'e', &FileSubCommand::open},
		{'r', &FileSubCommand::refresh},
	};
}

void FileSubCommand::ExecuteCommand() const {}

void FileSubCommand::moveUp() const {}

void FileSubCommand::moveDown() const {}

void FileSubCommand::open() const {}

void FileSubCommand::refresh() const {}