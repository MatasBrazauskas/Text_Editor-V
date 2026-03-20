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
	void ExecuteCommand() const;

  private:
	using Func = void (FileSubCommand::*)() const;

	void moveUp() const;
	void moveDown() const;

	void open() const;
	void refresh() const;

	std::unordered_map<char, Func> functionMap_;
};