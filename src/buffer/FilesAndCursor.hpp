#pragma once
#include "utils/FileHandler.hpp"

#include <filesystem>
#include <memory>
#include <stack>
#include <vector>

using namespace std::string_literals;

using FileId = uint_fast64_t;

inline constexpr int framesToSkip = 4;

class ITextBufferIterator {
public:
	ITextBufferIterator(const int index_t, const bool flag_t) : index_{index_t}, forwarded_{flag_t} {}
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

	[[nodiscard]] virtual const std::string_view getLine(int row) const = 0;

	[[nodiscard]] virtual const std::string_view getLineSubstr(int row, int col) const = 0;
	[[nodiscard]] virtual const std::string_view getLineSubstr(int row, int col, int n) const = 0;

	[[nodiscard]] virtual int getLineLength(int row) const = 0;

	[[nodiscard]] virtual int getLinesCount() const = 0;

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
	int absent_;
};

class File final {
      public:
	File() = delete;
	explicit File(std::unique_ptr<ITextBuffer>, std::filesystem::path);
	~File() noexcept = default;

	File(File&&) noexcept = default;
	File& operator=(File&&) noexcept = default;

	File(const File&) = delete;
	File& operator=(const File&) = delete;

	std::unique_ptr<ITextBuffer> textBuffer_;
	std::stack<int> undoStack_; // temp implement tho
	Cursor cursor_;

	std::filesystem::path filesPath_;
	FileId fileId_;
private:
    static inline int fileIdCounter_{0};
};

class FilesAndCursor final {
      public:
	FilesAndCursor(const FileHandler&, int argc, char** argv);
	~FilesAndCursor() noexcept = default;

	void addFile(std::unique_ptr<ITextBuffer>, std::filesystem::path);

	[[nodiscard]] std::reference_wrapper<File> getFile(size_t fileId_t);

	void removeFile(size_t fileId_t);

	std::vector<File> files_;
};