#include "Matrix.hpp"

void Matrix::init(std::vector<std::string> lines) {
    lines_ = std::move(lines);
}

std::string_view Matrix::row(size_t row) const {
    return lines_.at(row);
}

size_t Matrix::size() const {
    return lines_.size();
}
