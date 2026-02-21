#pragma once

#include "core/EditorCore.hpp"


#include <filesystem>
#include <stack>
#include <vector>

using namespace std::string_literals;

class FileHandler;

inline constexpr int framesToSkip = 4;

class MatrixIterator final {
public:
    MatrixIterator(const std::vector<std::string>& t_matrix, int t_index, bool t_flag);

    void next();
    std::string_view getLine() const;
    bool end(size_t) const;

private:
    const std::vector<std::string>& matrix_;

    int index_;
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
	FilesManager(const FileHandler&, int argc, char** argv);
	~FilesManager() noexcept = default;

	void addFile(const Matrix&, const std::filesystem::path&);

	[[nodiscard]] File& getFile(FileId);
	[[nodiscard]] File& getFile();

	std::unordered_map<FileId, File> files_;
	FileId activeFileId_;
	inline static FileId fileIdCounter_{0};
};