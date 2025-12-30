#include "Matrix.hpp"

#include <algorithm>
#include <string>

void Matrix::init(std::vector<std::string> lines, std::string separators) {
	lines_ = std::move(lines);
	separators_ = std::move(separators);
}

std::string_view Matrix::rowView(const size_t row) const {
	return lines_.at(row);
}

size_t Matrix::rowLength(const size_t row) const {
	return lines_.at(row).length();
}

std::string& Matrix::rowRef(const size_t row) {
	return lines_.at(row);
}

size_t Matrix::size() const {
	return lines_.size();
}

void Matrix::deleteLine(const size_t row) {
	lines_.erase(lines_.begin() + row);
}

void Matrix::deleteCharacter(const size_t row, const size_t col) {
	lines_.at(row).erase(col, 1);
}

std::optional<size_t> Matrix::firstCharOccurrenceRight(const size_t row, const size_t col, const char c) const {
	const auto rowLine = rowView(row);

	if (col + 1 >= rowLine.length()) {
		return std::nullopt;
	}

	const size_t index = rowLine.find(c, col + 1);

	if (index == std::string::npos || index <= col) {
		return std::nullopt;
	}

	return index;
}

std::optional<size_t> Matrix::firstCharOccurrenceLeft(size_t row, size_t col, char c) const {
	const auto rowLine = rowView(row);

	if (col - 1 < 0) {
		return std::nullopt;
	}

	const size_t index = rowLine.rfind(c, col - 1);

	if (index == std::string::npos || index >= col) {
		return std::nullopt;
	}

	return index;
}

std::optional<std::string_view> Matrix::nextWord(size_t row, size_t col) const {
	return std::nullopt;
}

std::optional<std::string_view> Matrix::prevWord(size_t row, size_t col) const {
	return std::nullopt;
}

void Matrix::insertLine(const size_t row) {
	lines_.insert(lines_.begin() + row, "");
}
