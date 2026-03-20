#pragma once

#include "buffer/PanesAndLayers.hpp"

#include <unordered_map>

class PanesManager;

class WindowSubCommand final {
  public:
	WindowSubCommand();
	~WindowSubCommand() noexcept = default;
	void ExecuteCommand(PanesManager&, char t_inputChar) const;

  private:
	using Func = void (WindowSubCommand::*)(PanesManager&) const;

	void verticalSplit(PanesManager&) const;
	void horizontalSplit(PanesManager&) const;

	void movePaneLeft(PanesManager&) const;
	void movePaneRight(PanesManager&) const;
	void movePaneDown(PanesManager&) const;
	void movePaneUp(PanesManager&) const;

	void closePane(PanesManager&) const;
	void equalizePanes(PanesManager&) const;

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