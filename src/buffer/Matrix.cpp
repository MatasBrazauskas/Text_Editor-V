#include "Matrix.hpp"

void Matrix::init(std::vector<std::string> lines) {
    lines_ = std::move(lines);
}

std::string_view Matrix::rowView(const size_t row) {
    return lines_.at(row);
}

std::string &Matrix::rowRef(const size_t row) {
    return lines_.at(row);
}

size_t Matrix::size() const {
    return lines_.size();
}
