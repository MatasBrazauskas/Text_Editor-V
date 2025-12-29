#pragma once

#include <vector>
#include <string>

#include "buffer/Files.hpp"

class Matrix final : public ITextBuffer {
public:
	Matrix() = default;

	void init(std::vector<std::string> lines, std::string separators) override;

	[[nodiscard]] std::string_view rowView(size_t row) const override;

	[[nodiscard]] size_t rowLength(size_t row) const override;

	[[nodiscard]] std::string& rowRef(size_t row) override;

	[[nodiscard]] size_t size() const override;

	void erase(size_t row) override;

	[[nodiscard]] std::optional<size_t> firstCharOccurrenceRight(size_t row, size_t col, char c) const override;

	[[nodiscard]] std::optional<size_t> firstCharOccurrenceLeft(size_t row, size_t col, char c) const override;

	[[nodiscard]] std::optional<std::string_view> nextWord(size_t row, size_t col) const override;

	[[nodiscard]] std::optional<std::string_view> prevWord(size_t row, size_t col) const override;

private:
	std::vector<std::string> lines_;
};