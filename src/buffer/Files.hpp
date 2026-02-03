#pragma once
#include "utils/FileHandler.hpp"

#include <filesystem>
#include <memory>
#include <optional>
#include <vector>

using namespace std::string_literals;

class Cursor final {
      public:
	Cursor();
	~Cursor() noexcept = default;

	void incrementX();
	void decrementX();
	void incrementY();
	void decrementY();

	[[nodiscard]] int getX() const;
	[[nodiscard]] int getY() const;

	void setX(int);
	void setY(int);

	[[nodiscard]] bool isVisible() const;
	void setVisible(bool);

      private:
	int x_;
	int y_;
	bool visible_;
};

class ITextBufferIterator {
      public:
	ITextBufferIterator(const int index_t, const bool flag_t) : index_(index_t), forwarded_(flag_t) {}
	virtual ~ITextBufferIterator() noexcept = default;

	virtual void next() = 0;
	virtual std::string_view getLine() = 0;
	virtual const bool end(size_t) const = 0;

	int index_;

      protected:
	std::string_view currLine_;
	bool forwarded_;
};

class ITextBuffer {
      public:
	ITextBuffer() = default;
	virtual ~ITextBuffer() noexcept = default;

	virtual void init(std::vector<std::string> matrix) = 0;

	[[nodiscard]] virtual const std::string_view rowsView(int row) const = 0;

	[[nodiscard]] virtual const std::string_view rowSubstr(int row, int col) const = 0;
	[[nodiscard]] virtual const std::string_view rowSubstr(int row, int col, int n) const = 0;

	[[nodiscard]] virtual int rowsLength(int row) const = 0;

	[[nodiscard]] virtual int linesCount() const = 0;

	virtual void deleteLine(int row) = 0;
	virtual void insertLine(int row, std::string) = 0;

	virtual void deleteCharacter(int row, int col) = 0;
	virtual void insertCharacter(int row, int col, char c) = 0;

	virtual void deleteRange(int row, int startCol, int len) = 0;
	virtual void insertRange(int row, int startCol, std::string_view range) = 0;

	virtual std::unique_ptr<ITextBufferIterator> forwardIterator(size_t) = 0;
	virtual std::unique_ptr<ITextBufferIterator> backwardIterator(size_t) = 0;

	inline static auto separators = "!@#$%^&*()-+={}[]:;'<>,.?/|\\\""s;

      protected:
	size_t rowsCount_;
	size_t charsCount_;
};

class TextBufferView final {
      public:
	TextBufferView();
	~TextBufferView() noexcept = default;

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
	explicit Document(std::unique_ptr<ITextBuffer>, std::filesystem::path);
	~Document() noexcept = default;

	Document(Document&&) noexcept = default;
	Document& operator=(Document&&) noexcept = default;

	Document(const Document&) = delete;
	Document& operator=(const Document&) = delete;

	std::unique_ptr<ITextBuffer> textBuffer_;
	TextBufferView textView_;
	Cursor cursor_;
	std::filesystem::path filesPath_;
};

class Files final {
      public:
	Files() = delete;
	explicit Files(const FileHandler&, int argc, char** argv);
	~Files() noexcept = default;

	void addDocument(std::unique_ptr<ITextBuffer>, std::filesystem::path);

	[[nodiscard]] std::optional<std::reference_wrapper<Document>> getDocument(size_t index_t);

	void removeDocument(size_t index_t);

      private:
	std::vector<Document> files_;
};