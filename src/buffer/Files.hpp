#pragma once
#include <vector>
#include <memory>
#include <optional>

#include "utils/FileHandler.hpp"

using namespace std::string_literals;

class Cursor final {
public:
	Cursor();
	~Cursor() = default;

	void incrementX();
	void decrementX();
	void incrementY();
	void decrementY();
	[[nodiscard]] int getX() const;
	[[nodiscard]] int getY() const;
	void setX(int);
	void setY(int);
	[[nodiscard]] bool isVisible() const;
private:
	int x_;
	int y_;
	bool visible_;
};

class ITextBuffer {
public :
	ITextBuffer() = default;

	virtual ~ITextBuffer() = default;

	std::string separators_;

	virtual void init(std::vector<std::string> matrix, std::string separators) = 0;

	[[nodiscard]] virtual std::string_view rowView(int row) const = 0;

	[[nodiscard]] virtual int rowsLength(int row) const = 0;

	[[nodiscard]] virtual int linesCount() const = 0;

	virtual void deleteLine(int row) = 0;
	virtual void insertLine(int row) = 0;

	virtual void deleteCharacter(int row, int col) = 0;
    virtual void insertCharacter(int row, int col, char c) = 0;

    virtual void deleteRange(int row, int startCol, int len) = 0;
    virtual void insertRange(int row, int startCol, std::string_view range) = 0;

};

class TextBufferView final {
public:
	TextBufferView();
	~TextBufferView() = default;

	int startY_;
    int startX_;

	int visibleLines_;
    int visibleColumns_;

	std::vector<int> dirtyLinesIndexes_;

	void clearDirtyLines();
	void addDirtyLine(int index);
};

class Document final {
public:
	Document() = delete;

	explicit Document(std::unique_ptr<ITextBuffer> textBuffer, std::string fileName);

    inline static auto separators = " ,./?<>!@#$%^&*()_-+=|[]{}:'"s;

	std::unique_ptr<ITextBuffer> textBuffer_;
	TextBufferView textView_;
	Cursor cursor_;
	std::string fileName_;
};

class Files final {
public:
	explicit Files(FileHandler&, int argc, char** argv);

	void addFrame(std::unique_ptr<ITextBuffer> textBuffer, std::string fileName);

	[[nodiscard]] Document& getDocument(size_t index) const;

	void removeDocument(size_t index);

	FileHandler& fileHandler_;
	//Some time change this to just stack value, no need to store smart pointer
	std::vector<std::unique_ptr<Document>> files_;
};