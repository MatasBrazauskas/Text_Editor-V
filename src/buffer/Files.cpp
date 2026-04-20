#include "Files.hpp"

#include <SDL.h>
#include <algorithm>
#include <numeric>

MatrixIterator::MatrixIterator(const std::vector<std::string>& t_matrix, const int t_index, const bool t_flag)
	: index_{t_index}, matrix_{t_matrix}, forwarded_{t_flag} {
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

std::string_view MatrixIterator::getLine() const {
	return currLine_;
}

bool MatrixIterator::end(const size_t endIndex_t) const {
	if (this->forwarded_) {
		return this->index_ >= endIndex_t;
	}
	return this->index_ < endIndex_t;
}

Matrix::Matrix(const std::vector<std::string>& t_lines) : dirty{false}, lines_{std::move(t_lines)}, charsCount_{} {
	if (lines_.size() == 0) {
		this->insertLine(0, "");
	}

	lineInfo_.reserve(lines_.size());
	for (int i = 0; i < lines_.size(); ++i) {
		lineInfo_.emplace_back(LineInfo::None);
	}

	const auto addLineCharCount = [](const int t_sum, const std::string& t_line) { return t_sum + t_line.length(); };

	charsCount_ = std::accumulate(lines_.begin(), lines_.end(), 0, addLineCharCount);
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
	charsCount_ -= lines_.at(row).length();
	lines_.erase(lines_.begin() + row);
	lineInfo_.erase(lineInfo_.begin() + row);

	if (lines_.empty()) {
		lines_.emplace_back("");
		lineInfo_.emplace_back(LineInfo::None);
	}
	dirty = true;
}

void Matrix::insertLine(const int row, const std::string t_line) {
	lines_.insert(lines_.begin() + row, t_line);
	lineInfo_.insert(lineInfo_.begin() + row, LineInfo::Insert);
	charsCount_ += t_line.length();
	dirty = true;
}

void Matrix::deleteCharacter(const int row, const int col) {
	lines_.at(row).erase(col, 1);
	lineInfo_[row] = LineInfo::Changed;
	charsCount_--;
	dirty = true;
}

void Matrix::insertCharacter(const int row, const int col, const char c) {
	lines_.at(row).insert(col, 1, c);
	lineInfo_[row] = LineInfo::Changed;
	charsCount_++;
	dirty = true;
}

void Matrix::deleteRange(const int row, const int startCol, const int len) {
	auto& line = lines_.at(row);
	line.erase(startCol, len);
	charsCount_ -= len;
	dirty = true;
}

void Matrix::insertRange(const int row, const int startCol, const std::string_view range) {
	auto& line = lines_.at(row);
	line.insert(startCol, std::string(range));
	lineInfo_[startCol] = LineInfo::Changed;
	charsCount_ += range.length();
	dirty = true;
}
MatrixIterator Matrix::forwardIterator(const size_t startCount_t) const {
	return MatrixIterator(this->lines_, startCount_t, true);
}

MatrixIterator Matrix::backwardIterator(const size_t startCount_t) const {
	return MatrixIterator(this->lines_, startCount_t, false);
}

File::File(const Matrix& text_t, std::filesystem::path path_t, const FileId t_fileId)
	: textBuffer_{std::move(text_t)}, filesPath_{std::move(path_t)}, fileId_{t_fileId} {}

FilesManager::FilesManager(const int argc, char** argv) : activeFileId_{}, specialFile_{std::nullopt} {
	if (argc < 1 || argv == nullptr) {
		return;
	}

	const std::vector<std::string_view> filePaths(argv + 1, argv + argc);

	if (filePaths.empty()) {
		auto ptr = Matrix({});
		this->addEmptyFile();
	} else {
		for (const auto& path : filePaths) {
			const auto lines = fileHandler_.readFile(path.data());
			auto ptr = Matrix(lines);

			this->addRegularFile(path);
		}
	}
}

FileId FilesManager::addRegularFile(const std::filesystem::path& t_filePath) {
	auto fileContents = Matrix{fileHandler_.readFile(t_filePath)};
	const auto regularFile = File{std::move(fileContents), t_filePath, fileIdCounter_};
	files_.push_back(std::move(regularFile));

	fileIdCounter_++;
	return regularFile.fileId_;
}

FileId FilesManager::addSpecialFile() {
	auto dirContents = Matrix{fileHandler_.readDirectory()};
	const auto currPath = std::filesystem::current_path() / "";
	const auto specialFile = File{std::move(dirContents), currPath, fileIdCounter_};
	files_.push_back(std::move(specialFile));

	fileIdCounter_++;
	specialFile_ = specialFile.fileId_;
	return specialFile.fileId_;
}

FileId FilesManager::addEmptyFile() {
	auto emptyText = Matrix{{""}};
	const std::filesystem::path someFilePath = std::filesystem::current_path() / "New Document";

	const auto regularFile = File{std::move(emptyText), someFilePath, fileIdCounter_};
	files_.push_back(std::move(regularFile));

	fileIdCounter_++;
	return regularFile.fileId_;
}

bool FilesManager::specialFile(const FileId t_fileId) const {
	return specialFile_ == t_fileId;
}

bool FilesManager::regularFile(const FileId t_fileId) const {
	return !specialFile(t_fileId);
}

void FilesManager::saveCurrentFile() {
	const auto file = getFile();
	fileHandler_.writeToFile(file);
}

File& FilesManager::getFile(const FileId t_fileId) {
	return files_.at(t_fileId);
}

File& FilesManager::getFile() {
	return files_.at(activeFileId_);
}

std::optional<FileId> FilesManager::getFileId(const std::string t_filename) {
	const auto predicate = [t_filename](const File& file) { return file.filesPath_.filename().string() == t_filename; };

	const auto it = std::ranges::find_if(files_.begin(), files_.end(), predicate);

	if (it == files_.end()) {
		return std::nullopt;
	}

	return it->fileId_;
}

void FilesManager::saveFile(const FileId t_fileId) {
	const auto file = getFile(t_fileId);
	fileHandler_.writeToFile(file);
}

FileId FilesManager::switchToNextFile() {
	const auto predicate = [this](const File& file) { return file.fileId_ == activeFileId_; };
	const auto it = std::ranges::find_if(files_, predicate);
	const auto index = (std::distance(files_.begin(), it) + 1) % (files_.size());

	activeFileId_ = files_.at(index).fileId_;
	return activeFileId_;
}

FileId FilesManager::switchToPrevFile() {
	const auto predicate = [this](const File& file) { return file.fileId_ == activeFileId_; };
	const auto it = std::ranges::find_if(files_, predicate);
	auto index = static_cast<int>(std::distance(files_.begin(), it) - 1);

	if (index < 0) {
		index = files_.size() - 1;
	}

	activeFileId_ = files_.at(index).fileId_;
	return activeFileId_;
}

void FilesManager::changeSpecialFile(const FileId t_fileId, const std::filesystem::path t_path) {
	auto dirContents = Matrix{fileHandler_.readDirectory(t_path)};
	const auto newPath = t_path / "";

	auto& file = this->getFile(t_fileId);
	file.filesPath_ = newPath;
	file.textBuffer_ = dirContents;
}

void FilesManager::removeFile(const FileId t_fileId) {
	const auto predicate = [t_fileId](const File& file) { return file.fileId_ == t_fileId; };

	std::ranges::remove_if(files_, predicate);

	activeFileId_ = files_.at(0).fileId_;
}
