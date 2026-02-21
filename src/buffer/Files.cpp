#include "Files.hpp"

#include "utils/FileHandler.hpp"

#include <SDL.h>
#include <algorithm>
#include <numeric>

MatrixIterator::MatrixIterator(const std::vector<std::string>& t_matrix, const int t_index, const bool t_flag)
    : matrix_{t_matrix}, index_{t_index}, forwarded_{t_flag} {
	this->currLine_ = matrix_.at(index_);
}

void MatrixIterator::next() {
	if (this->forwarded_) {
		this->index_++;
	} else {
		this->index_--;
	}

	if (matrix_.size() <= this->index_ || 0 > this->index_) {
		return;
	}
	this->currLine_ = matrix_.at(this->index_);
}

std::string_view MatrixIterator::getLine()const {
	return currLine_;
}

bool MatrixIterator::end(const size_t endIndex_t) const {
	if (this->forwarded_) {
		return this->index_ >= endIndex_t;
	}
	return this->index_ < endIndex_t;
}

Matrix::Matrix(const std::vector<std::string>& t_lines) : lines_{std::move(t_lines)}, charsCount_{} {
    if (lines_.size() == 1 && lines_.at(0).empty()) {
        lines_.push_back("");
    }
}

std::string_view Matrix::getLine(const int row) const {
	const auto& line = lines_.at(row);
	return std::string_view{line};
}

std::string_view Matrix::getLineSubstr(const int row, const int col, const int n) const {
	const auto& line = lines_.at(row);
	return std::string_view{line.data() + col, static_cast<size_t>(n)};
}

std::string_view Matrix::getLineSubstr(const int row, const int col) const {
	const auto& line = lines_.at(row);
	return std::string_view{line.data() + col};
}

int Matrix::getLineLength(const int row) const {
	return lines_.at(row).length();
}

int Matrix::getLinesCount() const {
	return lines_.size();
}

int Matrix::getCharCount() const {
    return charsCount_;
}

void Matrix::deleteLine(const int row) {
	lines_.erase(lines_.begin() + row);

	if (lines_.empty()) {
		lines_.emplace_back("");
	}
}

void Matrix::insertLine(const int row, const std::string line) {
	lines_.insert(lines_.begin() + row, line);
}

void Matrix::deleteCharacter(const int row, const int col) {
	lines_.at(row).erase(col, 1);
}

void Matrix::insertCharacter(const int row, const int col, const char c) {
	lines_.at(row).insert(col, 1, c);
}

void Matrix::deleteRange(const int row, const int startCol, const int len) {
	auto& line = lines_.at(row);
	line.erase(startCol, len);
}

void Matrix::insertRange(const int row, const int startCol, const std::string_view range) {
	auto& line = lines_.at(row);
	line.insert(startCol, std::string(range));
}
MatrixIterator Matrix::forwardIterator(const size_t startCount_t) const {
	return MatrixIterator(this->lines_, startCount_t, true);
}

MatrixIterator Matrix::backwardIterator(const size_t startCount_t) const {
	return MatrixIterator(this->lines_, startCount_t, false);
}

File::File(const Matrix& text_t, std::filesystem::path path_t, const FileId t_fileId)
    : textBuffer_{std::move(text_t)}, filesPath_{std::move(path_t)}, fileId_ {t_fileId} {}

FilesManager::FilesManager(const FileHandler& fileHandler, const int argc, char** argv): activeFileId_{} {
	if (argc < 1 || argv == nullptr) {
		return;
	}

	const std::vector<std::string_view> filePaths(argv + 1, argv + argc);

	if (filePaths.empty()) {
		auto ptr = Matrix({});
		this->addFile(std::move(ptr), "Untitled");
	} else {
		for (const auto& path : filePaths) {
			const auto lines = fileHandler.getContent(path.data());
			auto ptr = Matrix(lines);

			this->addFile(std::move(ptr), path);
		}
	}
}

void FilesManager::addFile(const Matrix& textBuffer, const std::filesystem::path& filePath_t) {
	const auto file = File{std::move(textBuffer), filePath_t, fileIdCounter_};
	files_.insert({fileIdCounter_, file});
	fileIdCounter_++;
}

File& FilesManager::getFile(const FileId t_fileId) {
	return files_.at(t_fileId);
}

File& FilesManager::getFile() {
	return  files_.at(activeFileId_);
}
