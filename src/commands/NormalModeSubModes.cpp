#include "NormalModeSubModes.hpp"

WindowSubCommand::WindowSubCommand() {

	functionMap_ = {{'v', &WindowSubCommand::verticalSplit}, {'s', &WindowSubCommand::horizontalSplit},
					{'h', &WindowSubCommand::movePaneLeft},  {'j', &WindowSubCommand::movePaneDown},
					{'k', &WindowSubCommand::movePaneUp},	  {'l', &WindowSubCommand::movePaneRight},
					{'c', &WindowSubCommand::closePane}};
}
void WindowSubCommand::ExecuteCommand(PanesManager& t_panesManager, const char t_inputChar) const{

	const auto it = functionMap_.find(t_inputChar);
	if (it != functionMap_.end()) {
		(this->*it->second)(t_panesManager);
	}
}

void WindowSubCommand::verticalSplit(PanesManager& t_panesManager) const {
	const auto& pane = t_panesManager.getCurrPane();
	t_panesManager.addPane(pane->paneId_, pane->fileId_, AddedPaneRotation::Left);
}

void WindowSubCommand::horizontalSplit(PanesManager& t_panesManager) const {
	const auto& pane = t_panesManager.getCurrPane();
	t_panesManager.addPane(pane->paneId_, pane->fileId_, AddedPaneRotation::Bottom);
}

void WindowSubCommand::movePaneLeft(PanesManager& t_panesManager) const {
}

void WindowSubCommand::movePaneRight(PanesManager&) const {}

void WindowSubCommand::movePaneDown(PanesManager&) const {}

void WindowSubCommand::movePaneUp(PanesManager&) const {}

void WindowSubCommand::closePane(PanesManager& t_panesManager) const {
	const auto paneId = t_panesManager.getCurrPane()->paneId_;
	t_panesManager.removePane(paneId);
}

void WindowSubCommand::equalizePanes(PanesManager& t_panesManager) const {
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

void FileSubCommand::ExecuteCommand() const {

}

void FileSubCommand::moveUp() const {

}

void FileSubCommand::moveDown() const {

}

void FileSubCommand::open() const {

}

void FileSubCommand::refresh() const {

}