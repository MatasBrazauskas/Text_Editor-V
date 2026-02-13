#pragma once

#include "buffer/FilesAndCursor.hpp"

#include <string>
#include <vector>

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

	[[nodiscard]] const std::string_view getLine(int row) const override;
	[[nodiscard]] const std::string_view getLineSubstr(int row, int col) const override;
	[[nodiscard]] const std::string_view getLineSubstr(int row, int col, int n) const override;

	[[nodiscard]] int getLineLength(int row) const override;

	[[nodiscard]] int getLinesCount() const override;

	void deleteLine(int row) override;
	void insertLine(int row, std::string line) override;

	void deleteCharacter(int row, int col) override;
	void insertCharacter(int row, int col, char c) override;

	void deleteRange(int row, int startCol, int len) override;
	void insertRange(int row, int startCol, std::string_view range) override;

	[[nodiscard]] std::unique_ptr<ITextBufferIterator> forwardIterator(size_t) override;
	[[nodiscard]] std::unique_ptr<ITextBufferIterator> backwardIterator(size_t) override;

	std::vector<std::string> lines_;
	std::unique_ptr<ITextBufferIterator> iter_;
};