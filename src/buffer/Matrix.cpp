#include "Matrix.hpp"

#include <algorithm>
#include <string>

Matrix::Matrix() {
	lines_ = {""};
}

void Matrix::init(std::vector<std::string> lines, std::string separators) {
	lines_ = std::move(lines);
	separators_ = std::move(separators);
}

std::string_view Matrix::rowView(const size_t row) const {
	return lines_.at(row);
}

size_t Matrix::rowsLength(const size_t row) const {
	return lines_.at(row).length();
}

size_t Matrix::linesCount() const {
	return lines_.size();
}

void Matrix::deleteLine(const size_t row) {
	lines_.erase(lines_.begin() + row);

    if (lines_.empty()) {
        lines_.emplace_back("");
    }
}

void Matrix::insertLine(const size_t row) {
	lines_.insert(lines_.begin() + row, "");
}

void Matrix::deleteCharacter(const size_t row, const size_t col) {
    lines_.at(row).erase(col, 1);
}

void Matrix::insertCharacter(const size_t row, const size_t col, const char c) {
	lines_.at(row).insert(col, 1, c);
}
