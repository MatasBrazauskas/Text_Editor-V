#pragma once
#include <vector>
#include <memory>
#include <optional>

class Cursor final {
public:
	Cursor();
	~Cursor() = default;

	void        incrementX();
	void        decrementX();
	void        incrementY();
	void        decrementY();
	std::size_t getX() const;
	std::size_t getY() const;
	void        setX(std::size_t);
	void        setY(std::size_t);
	bool        isVisible() const;
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

	[[nodiscard]] virtual size_t rowLength(size_t row) const = 0;

	[[nodiscard]] virtual std::string& rowRef(size_t row) = 0;

	[[nodiscard]] virtual size_t size() const = 0;

	virtual void deleteLine(size_t row) = 0;
	virtual void deleteCharacter(size_t row, size_t col) = 0;

	[[nodiscard]] virtual std::optional<size_t> firstCharOccurrenceRight(size_t row, size_t col, char c) const = 0;

	[[nodiscard]] virtual std::optional<size_t> firstCharOccurrenceLeft(size_t row, size_t col, char c) const = 0;

	[[nodiscard]] virtual std::optional<std::string_view> nextWord(size_t row, size_t col) const = 0;

	[[nodiscard]] virtual std::optional<std::string_view> prevWord(size_t row, size_t col) const = 0;

	virtual void insertLine(size_t row) = 0;
};

class Document final {
public:
	Document() = delete;

	explicit Document(std::unique_ptr<ITextBuffer> textBuffer, std::string fileName);

	std::unique_ptr<ITextBuffer> textBuffer_;
	Cursor cursor_;
	std::string fileName_;
};

class Files final {
public:
	Files() = default;

	void addFrame(std::unique_ptr<ITextBuffer> textBuffer, std::string fileName);

	[[nodiscard]] Document& getDocument(size_t index) const;

	std::vector<std::unique_ptr<Document>> files_;
};