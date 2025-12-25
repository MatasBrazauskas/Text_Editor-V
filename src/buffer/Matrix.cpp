#include "Matrix.hpp"
#include <iostream>

#include <algorithm>
#include <string>

void Matrix::init(std::vector<std::string> lines, std::string separators) {
	lines_ = std::move(lines);
	separators_ = separators;
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

void Matrix::erase(const size_t row) {
	lines_.erase(lines_.begin() + row);
}

std::optional<size_t> Matrix::firstCharAccuranceRight(size_t row, size_t col, char c) const {
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

std::optional<size_t> Matrix::firstCharAccuranceLeft(size_t row, size_t col, char c) const {
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
