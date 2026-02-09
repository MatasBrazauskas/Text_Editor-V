#include "Matrix.hpp"

#include <algorithm>
#include <string>

MatrixIterator::MatrixIterator(std::vector<std::string>& ds_t, const int index_t, const bool flag_t)
    : ITextBufferIterator(index_t, flag_t), lines_{ds_t} {
	this->currLine_ = ds_t.at(index_t);
}

void MatrixIterator::next() {
	if (this->forwarded_) {
		this->index_++;
	} else {
		this->index_--;
	}

	if (lines_.size() <= this->index_ || 0 > this->index_) {
		return;
	}
	this->currLine_ = lines_.at(this->index_);
}

std::string_view MatrixIterator::getLine() {
	return currLine_;
}

const bool MatrixIterator::end(const size_t endIndex_t) const {
	if (this->forwarded_) {
		return this->index_ >= endIndex_t;
	}
	return this->index_ < endIndex_t;
}

Matrix::Matrix() : lines_{""} {}

void Matrix::init(std::vector<std::string> lines) {
	// lines_ = std::move(lines);

    if (lines_.size() == 1 && lines_.at(0).empty()) {
        lines_ = std::move(lines);
    } else {
        std::ranges::copy(lines, back_inserter(lines_));
    }
}

const std::string_view Matrix::rowsView(const int row) const {
	const auto& line = lines_.at(row);
	return std::string_view{line};
}

const std::string_view Matrix::rowSubstr(const int row, const int col, const int n) const {
	const auto& line = lines_.at(row);
	return std::string_view{line.data() + col, static_cast<size_t>(n)};
}

const std::string_view Matrix::rowSubstr(const int row, const int col) const {
	const auto& line = lines_.at(row);
	return std::string_view{line.data() + col};
}

int Matrix::rowsLength(const int row) const {
	return lines_.at(row).length();
}

int Matrix::linesCount() const {
	return lines_.size();
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
std::unique_ptr<ITextBufferIterator> Matrix::forwardIterator(const size_t startCount_t) {
	return std::make_unique<MatrixIterator>(this->lines_, startCount_t, true);
}

std::unique_ptr<ITextBufferIterator> Matrix::backwardIterator(const size_t startCount_t) {
	return std::make_unique<MatrixIterator>(this->lines_, startCount_t, false);
}
