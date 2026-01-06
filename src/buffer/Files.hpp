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
	[[nodiscard]] std::size_t getX() const;
	[[nodiscard]] std::size_t getY() const;
	void setX(std::size_t);
	void setY(std::size_t);
	[[nodiscard]] bool isVisible() const;
private:
	std::size_t x_;
	std::size_t y_;
	bool visible_;
};

class ITextBuffer {
public :
	ITextBuffer() = default;

	virtual ~ITextBuffer() = default;

	std::string separators_;

	virtual void init(std::vector<std::string> matrix, std::string separators) = 0;

	[[nodiscard]] virtual std::string_view rowView(size_t row) const = 0;

	[[nodiscard]] virtual size_t rowsLength(size_t row) const = 0;

	[[nodiscard]] virtual size_t linesCount() const = 0;

	virtual void deleteLine(size_t row) = 0;
	virtual void insertLine(size_t row) = 0;

	virtual void deleteCharacter(size_t row, size_t col) = 0;
    virtual void insertCharacter(size_t row, size_t col, char c) = 0;

};

class TextBufferView final {
public:
	TextBufferView();
	~TextBufferView() = default;

	std::size_t startY_;
    std::size_t startX_;

	std::size_t visibleLines_;
    std::size_t visibleColumns_;

	std::vector<std::size_t> dirtyLinesIndexes_;

	void clearDirtyLines();
	void addDirtyLine(std::size_t index);
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