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

std::string_view Matrix::rowView(const int row) const {
	return lines_.at(row);
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

void Matrix::insertLine(const int row) {
	lines_.insert(lines_.begin() + row, "");
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
    line.insert(startCol, range);
}
