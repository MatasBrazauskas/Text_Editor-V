#include "Files.hpp"

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

std::size_t Cursor::getX() const {
	return x_;
}

std::size_t Cursor::getY() const {
	return y_;
}

void Cursor::setX(const std::size_t x) {
	x_ = x;
}

void Cursor::setY(const std::size_t y) {
	y_ = y;
}

bool Cursor::isVisible() const {
	return visible_;
}

Document::Document(std::unique_ptr<ITextBuffer> textBuffer, std::string fileName): textBuffer_(std::move(textBuffer)), fileName_(std::move(fileName)) {}

void Files::addFrame(std::unique_ptr<ITextBuffer> textBuffer, std::string fileName) {
	auto doc = std::make_unique<Document>(std::move(textBuffer), std::move(fileName));
	files_.push_back(std::move(doc));
}

Document& Files::getDocument(const size_t index) const {
	return *files_.at(index);
}