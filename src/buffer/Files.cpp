#include "Files.hpp"

#include "Matrix.hpp"

#include <SDL.h>
#include <algorithm>
#include <numeric>

Cursor::Cursor() : x_{0}, y_{0}, visible_{true}, absent_{} {}

void Cursor::incrementX() {
	this->setX(x_ + 1);
}

void Cursor::decrementX() {
	this->setX(x_ - 1);
}

void Cursor::incrementY() {
	this->setY(y_ + 1);
}

void Cursor::decrementY() {
	this->setY(y_ - 1);
}

int Cursor::getX() const {
	return x_;
}

int Cursor::getY() const {
	return y_;
}

void Cursor::setX(const int x) {
	x_ = x;
	visible_ = true;
	absent_ = framesToSkip;
}

void Cursor::setY(const int y) {
	y_ = y;
	visible_ = true;
	absent_ = framesToSkip;
}

bool Cursor::isVisible() const {
	return visible_;
}

void Cursor::setVisible(const bool visible) {
	visible_ = visible;
}

File::File(std::unique_ptr<ITextBuffer> text_t, std::filesystem::path path_t, const FileId t_fileId)
    : textBuffer_{std::move(text_t)}, filesPath_{std::move(path_t)}, fileId_{t_fileId} {}

FilesManager::FilesManager(const FileHandler& fileHandler, const int argc, char** argv) {
	if (argc < 1 || argv == nullptr) {
		return;
	}

	const std::vector<std::string_view> filePaths(argv + 1, argv + argc);

	if (filePaths.empty()) {
		auto ptr = std::make_unique<Matrix>();

		this->addFile(std::move(ptr), "Untitled");
	} else {
		for (const auto& path : filePaths) {
			const auto lines = fileHandler.getContent(path.data());

			auto ptr = std::make_unique<Matrix>();
			ptr->init(lines);

			this->addFile(std::move(ptr), path);
		}
	}
}

void FilesManager::addFile(std::unique_ptr<ITextBuffer> textBuffer, std::filesystem::path filePath_t) {
    files_.emplace_back(std::move(textBuffer), std::move(filePath_t), fileIdCounter_);
    fileIdCounter_++;
}

std::reference_wrapper<File> FilesManager::getFile(const FileId fileId_t) {
    const auto predicate = [fileId_t](const File& file) { return file.fileId_ == fileId_t; };
    const auto it = std::ranges::find_if(files_, predicate);

    if (it == files_.end()) {
        throw std::out_of_range("File not found");
    }

    return *it;
}

void FilesManager::removeFile(const size_t fileId_t) {
    const auto predicate = [fileId_t](const File& file) { return file.fileId_ == fileId_t; };
    const auto it = std::ranges::find_if(files_, predicate);

    if (it == files_.end()) {
        throw std::out_of_range("File not found when  removing");
    }
    files_.erase(it);
}
