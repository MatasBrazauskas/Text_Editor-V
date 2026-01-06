#pragma once

#include <vector>
#include <string>

#include "buffer/Files.hpp"

class Matrix final : public ITextBuffer {
public:
	Matrix();

	void init(std::vector<std::string> lines, std::string separators) override;

	[[nodiscard]] std::string_view rowView(size_t row) const override;

	[[nodiscard]] size_t rowsLength(size_t row) const override;

	[[nodiscard]] size_t linesCount() const override;

	void deleteLine(size_t row) override;
    void insertLine(size_t row) override;

	void deleteCharacter(size_t row, size_t col) override;
    void insertCharacter(size_t row, size_t col, char c) override;


private:
	std::vector<std::string> lines_;
};