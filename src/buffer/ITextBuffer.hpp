#pragma once

#include <cstddef>
#include <vector>
#include <string>
#include <string_view>
#include <optional>

class ITextBuffer {
public :
	virtual ~ITextBuffer() = default;

	[[nodiscard]] virtual std::string_view rowView(size_t row) const = 0;

	[[nodiscard]] virtual size_t rowLength(size_t row) const = 0;

	[[nodiscard]] virtual std::string& rowRef(size_t row) = 0;

	[[nodiscard]] virtual size_t size() const = 0;

	virtual void erase(size_t row) = 0;

	virtual void init(std::vector<std::string> matrix, std::string separators) = 0;

	[[nodiscard]] virtual std::optional<size_t> firstCharAccuranceRight(size_t row, size_t col, char c) const = 0;

	[[nodiscard]] virtual std::optional<size_t> firstCharAccuranceLeft(size_t row, size_t col, char c) const = 0;

	std::string separators_;
};