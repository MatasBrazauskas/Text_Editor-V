#pragma once

#include <vector>
#include <string>

#include "buffer/Files.hpp"

class MatrixIterator final : public ITextBufferIterator {
public:
    MatrixIterator(std::vector<std::string>& ds_t, int index_t, bool flag_t);

    void next() override;
    std::string_view getLine() override;
    const bool end(size_t) const override;
private:
    std::vector<std::string>& lines_;
};

class Matrix final : public ITextBuffer {
public:
	Matrix();

	void init(std::vector<std::string> lines) override;

	[[nodiscard]] const std::string_view rowsView(int row) const override;
    [[nodiscard]] const std::string_view rowSubstr(int row, int col) const override;
    [[nodiscard]] const std::string_view rowSubstr(int row, int col, int n) const override;

	[[nodiscard]] int rowsLength(int row) const override;

	[[nodiscard]] int linesCount() const override;

	void deleteLine(int row) override;
    void insertLine(int row) override;

	void deleteCharacter(int row, int col) override;
    void insertCharacter(int row, int col, char c) override;

    void deleteRange(int row, int startCol, int len) override;
    void insertRange(int row, int startCol, std::string_view range) override;

    [[nodiscard]] std::unique_ptr<ITextBufferIterator> forwardIterator(size_t) override;
    [[nodiscard]] std::unique_ptr<ITextBufferIterator> backwardIterator(size_t) override;

	std::vector<std::string> lines_;
    std::unique_ptr<ITextBufferIterator> iter_;
};