#pragma once

#include <vector>
#include <string>

#include "buffer/Files.hpp"

class Matrix final : public ITextBuffer {
public:
	Matrix();

	void init(std::vector<std::string> lines, std::string separators) override;

	[[nodiscard]] std::string_view rowView(int row) const override;

	[[nodiscard]] int rowsLength(int row) const override;

	[[nodiscard]] int linesCount() const override;

	void deleteLine(int row) override;
    void insertLine(int row) override;

	void deleteCharacter(int row, int col) override;
    void insertCharacter(int row, int col, char c) override;

    void deleteRange(int row, int startCol, int len) override;
    void insertRange(int row, int startCol, std::string_view range) override;


private:
	std::vector<std::string> lines_;
};