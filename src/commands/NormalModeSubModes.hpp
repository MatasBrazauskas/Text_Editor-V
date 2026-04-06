#pragma once

#include "buffer/PanesAndLayers.hpp"

#include <unordered_map>

class PanesManager;
class WindowSettings;

class WindowSubCommand final {
  public:
	WindowSubCommand();
	~WindowSubCommand() noexcept = default;
	void ExecuteCommand(PanesManager&, WindowSettings&, char t_inputChar) const;

  private:
	using Func = void (WindowSubCommand::*)(PanesManager&, WindowSettings&) const;

	void verticalSplit(PanesManager&, WindowSettings&) const;
	void horizontalSplit(PanesManager&, WindowSettings&) const;

	void movePaneLeft(PanesManager&, WindowSettings&) const;
	void movePaneRight(PanesManager&, WindowSettings&) const;
	void moveToPaneDown(PanesManager&, WindowSettings&) const;
	void movePaneUp(PanesManager&, WindowSettings&) const;

	void closePane(PanesManager&, WindowSettings&) const;
	void equalizePanes(PanesManager&, WindowSettings&) const;

	std::unordered_map<char, Func> functionMap_;
};

class FileSubCommand final {
  public:
	FileSubCommand();
	~FileSubCommand() noexcept = default;
	void ExecuteCommand(PanesManager&, FilesManager&, WindowSettings&, char t_inputChar) const;

  private:
	using Func = void (FileSubCommand::*)(PanesManager&, FilesManager&, WindowSettings&) const;

	void openInVertical(PanesManager&, FilesManager&, WindowSettings&) const;
	void openInHorizontal(PanesManager&, FilesManager&, WindowSettings&) const;
	void moveUp(PanesManager&, FilesManager&, WindowSettings&) const;
	void moveDown(PanesManager&, FilesManager&, WindowSettings&) const;

	void open() const;
	void close() const;
	void refresh() const;

	std::unordered_map<char, Func> functionMap_;
};