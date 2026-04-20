#pragma once

#include "utils/FileHandler.hpp"

#include <filesystem>
#include <optional>
#include <stack>
#include <vector>

using namespace std::string_literals;

using FileId = std::uint_fast64_t;

enum class LineInfo : char { None, Insert, Changed };

class MatrixIterator final {
  public:
	MatrixIterator(const std::vector<std::string>& t_matrix, int t_index, bool t_flag);

	void next();
	std::string_view getLine() const;
	bool end(size_t) const;
	int index_;

  private:
	const std::vector<std::string>& matrix_;

	std::string_view currLine_;
	bool forwarded_;
};

class Matrix final {
  public:
	explicit Matrix(const std::vector<std::string>& t_lines);

	[[nodiscard]] std::string_view getLine(int row) const;
	[[nodiscard]] std::string_view getLineSubstr(int row, int col) const;
	[[nodiscard]] std::string_view getLineSubstr(int row, int col, int n) const;

	[[nodiscard]] int getLineLength(int row) const;

	[[nodiscard]] int getLinesCount() const;
	[[nodiscard]] int getCharCount() const;

	void deleteLine(int row);
	void insertLine(int row, std::string line);

	void deleteCharacter(int row, int col);
	void insertCharacter(int row, int col, char c);

	void deleteRange(int row, int startCol, int len);
	void insertRange(int row, int startCol, std::string_view range);

	[[nodiscard]] MatrixIterator forwardIterator(size_t) const;
	[[nodiscard]] MatrixIterator backwardIterator(size_t) const;

	static constexpr std::string separators_ = "<>?/";
	std::vector<LineInfo> lineInfo_;
	bool dirty;

  private:
	std::vector<std::string> lines_;
	int charsCount_;
};

class File final {
  public:
	File() = delete;
	explicit File(const Matrix&, std::filesystem::path, FileId);
	~File() noexcept = default;

	Matrix textBuffer_;
	std::stack<int> undoStack_; // temp implement tho

	std::filesystem::path filesPath_;
	FileId fileId_;
};

class FilesManager final {
  public:
	FilesManager(int argc, char** argv);
	~FilesManager() noexcept = default;

	FileId addRegularFile(const std::filesystem::path&);
	FileId addSpecialFile();
	FileId addEmptyFile();

	void removeFile(FileId);

	bool specialFile(FileId) const;
	bool regularFile(FileId) const;

	void saveCurrentFile();
	void saveFile(FileId);

	[[nodiscard]] std::optional<FileId> getFileId(std::string);
	[[nodiscard]] File& getFile(FileId);
	[[nodiscard]] File& getFile();

	[[nodiscard]] FileId switchToNextFile();
	[[nodiscard]] FileId switchToPrevFile();

	void changeSpecialFile(FileId, std::filesystem::path);

	std::vector<File> files_;
	FileId activeFileId_;
	inline static FileId fileIdCounter_{0};

	FileHandler fileHandler_;
	std::optional<FileId> specialFile_;
};