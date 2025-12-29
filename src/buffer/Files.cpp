#include "Files.hpp"

Cursor::Cursor(): x_{}, y_{}, visible_{true} {}

Document::Document(std::unique_ptr<ITextBuffer> textBuffer): textBuffer_(std::move(textBuffer)) {}

void Files::addFrame(std::unique_ptr<ITextBuffer> textBuffer) {
	auto doc = std::make_unique<Document>(std::move(textBuffer));
	files_.push_back(std::move(doc));
}

Document& Files::getDocument(const size_t index) const {
	return *files_.at(index);
}