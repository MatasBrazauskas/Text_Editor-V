#include "Files.hpp"

#include "Matrix.hpp"

#include <SDL.h>

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

TextBufferView::TextBufferView() : startY_{0}, startX_{0}, visibleLines_{}, visibleColumns_{} {
	visibleLines_ = 28;
	visibleColumns_ = 85;
}

Document::Document(std::unique_ptr<ITextBuffer> textBuffer_t, std::filesystem::path fileName_t)
    : textBuffer_(std::move(textBuffer_t)), filesPath_(std::move(fileName_t)) {}

void Files::addDocument(std::unique_ptr<ITextBuffer> textBuffer, std::filesystem::path filePath_t) {
	files_.emplace_back(std::move(textBuffer), std::move(filePath_t));
}

Files::Files(const FileHandler& fileHandler, const int argc, char** argv) {
	if (argc < 1 || argv == nullptr) {
		return;
	}

	if (const std::vector<std::string_view> files(argv + 1, argv + argc); files.empty()) {
		auto ptr = std::make_unique<Matrix>();

		addDocument(std::move(ptr), "Untitled");
	} else {
		for (const auto& file : files) {
			const auto lines = fileHandler.getContent(file.data());

			auto ptr = std::make_unique<Matrix>();
			ptr->init(lines);

			addDocument(std::move(ptr), file);
		}
	}
}

std::optional<std::reference_wrapper<Document>> Files::getDocument(const size_t index_t) {
	if (index_t < files_.size()) {
		return files_.at(index_t);
	}
	return std::nullopt;
}

void Files::removeDocument(const size_t index_t) {
	if (index_t < files_.size()) {
		files_.erase(files_.begin() + index_t);
	}
}