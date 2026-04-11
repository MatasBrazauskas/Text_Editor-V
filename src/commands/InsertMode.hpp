#pragma once

#include <unordered_map>

class EditorInputAndOutput;
class File;
class Cursor;
class Config;

class InsertMode final {
  public:
	InsertMode();
	~InsertMode() = default;

	void HandleKeyboardInput(EditorInputAndOutput&, File&, Cursor&, const Config&) const;
	using Func = void (InsertMode::*)(File&, Cursor&, const Config&) const;

  private:
	void handleEnter(File&, Cursor&, const Config&) const;
	void handleBackspace(File&, Cursor&, const Config&) const;
	void handleTab(File&, Cursor&, const Config&) const;

	std::unordered_map<char, Func> commands_;
};