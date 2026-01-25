#pragma once

#include <SDL.h>
#include <SDL_ttf.h>

#include "utils/Config.hpp"
#include "core/Editor.hpp"

class Renderer final {
public:
	Renderer() = delete;

	Renderer(const EditorState&, Files&, const Config&);

	~Renderer();

	void Render();

	int charWidth_;
	int charHeight_;

	int windowWidth_;
	int windowHeight_;

	SDL_Window*   window_;
	SDL_Renderer* renderer_;
	TTF_Font*     font_;

	const EditorState& editorState_;
	Files& files_;
	const Config& config_;
private:
	void RenderText() const;
    void RenderLine(std::string_view, int) const;
	void RenderCursor() const;
	void RenderCommandLine() const;
};