#include "Files.hpp"

#include "Matrix.hpp"

#include <SDL.h>

Cursor::Cursor(): x_{}, y_{}, visible_{true} {}

void Cursor::incrementX() {
	x_++;
}

void Cursor::decrementX() {
	x_--;
}

void Cursor::incrementY() {
	y_++;
}

void Cursor::decrementY() {
	y_--;
}

int Cursor::getX() const {
	return x_;
}

int Cursor::getY() const {
	return y_;
}

void Cursor::setX(const int x) {
	x_ = x;
}

void Cursor::setY(const int y) {
	y_ = y;
}

bool Cursor::isVisible() const {
	return visible_;
}

TextBufferView::TextBufferView():startY_{}, startX_{}, visibleLines_{}, visibleColumns_{} {
	visibleLines_ = 28;
    visibleColumns_ = 80;
}

void TextBufferView::clearDirtyLines() {
	dirtyLinesIndexes_.clear();
}

void TextBufferView::addDirtyLine(const int index) {
	dirtyLinesIndexes_.push_back(index);
}

Document::Document(std::unique_ptr<ITextBuffer> textBuffer, std::string fileName): textBuffer_(std::move(textBuffer)), fileName_(std::move(fileName)) {}

void Files::addFrame(std::unique_ptr<ITextBuffer> textBuffer, std::string fileName) {
	auto doc = std::make_unique<Document>(std::move(textBuffer), std::move(fileName));
	files_.push_back(std::move(doc));
}

Files::Files(FileHandler& fileHandler, const int argc, char** argv): fileHandler_{fileHandler}{
	const std::vector<std::string> files(argv + 1, argv+ argc);

	if (files.empty()) {
		auto ptr = std::make_unique<Matrix>();
		addFrame(std::move(ptr), "New Document.txt");
	} else {
		for (const auto& file: files) {
			const auto lines = fileHandler_.getContent(file.c_str());
			auto ptr = std::make_unique<Matrix>();
			ptr->init(lines, file);
			addFrame(std::move(ptr), file);
		}
	}
}

Document& Files::getDocument(const size_t index) const {
	return *files_.at(index);
}

void Files::removeDocument(const size_t index) {
	files_.erase(files_.begin() + static_cast<int>(index));
}