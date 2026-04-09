/*#include "utils/Config.hpp"

#include <buffer/Matrix.hpp>
#include <commands/NormalMode.hpp>
#include <core/EditorCore.hpp>
#include <gtest/gtest.h>

struct Param {
	int curX, curY;
	int x, y;
	NormalModeCommand command;
};

class NormalModeMotions : public testing::Test {
	  protected:
	NormalMode normalMode;
	std::unique_ptr<ITextBuffer> matrix;
	const std::filesystem::path inputPath = "temp";
	Document doc;
	EditorState state{};

	NormalModeMotions() : doc{std::move(matrix), inputPath} {

		matrix = std::make_unique<Matrix>();
		doc = Document(std::move(matrix), inputPath);
		doc.cursor_.setX(0);
		doc.cursor_.setY(0);

		doc.textBuffer_->insertLine(0, " ...Hello World.");
		doc.textBuffer_->insertLine(1, "Hello World ..a   ");
	}

	void SetUp() override {
		matrix = std::make_unique<Matrix>();
		doc = Document(std::move(matrix), inputPath);
		doc.cursor_.setX(0);
		doc.cursor_.setY(0);

		doc.textBuffer_->insertLine(0, " ...Hello World.");
		doc.textBuffer_->insertLine(1, "Hello World ..a   ");
	}
};

class NormalModeMotionsTest : public NormalModeMotions, public testing::WithParamInterface<Param> {};

TEST_P(NormalModeMotionsTest, HandleLotsOfCases) {
	const auto& [curX, curY, x, y, command] = GetParam();

	this->doc.cursor_.setX(curX);
	this->doc.cursor_.setY(curY);

	const NormalModeTable table{};
	NormalModeExecutor executor{table};

	executor.executeNormalModeCommand(doc.textBuffer_, doc.cursor_, state, command);

	ASSERT_EQ(doc.cursor_.getX(), x);
	ASSERT_EQ(doc.cursor_.getY(), y);
}

INSTANTIATE_TEST_SUITE_P(
	WordMotionCommands, NormalModeMotionsTest,
	::testing::Values(Param{0, 0, 1, 0, NormalModeCommand{0, ' ', 0, 'W', ' ', ' ', false, ParsingStages::Finish}},
			  Param{1, 0, 10, 0, NormalModeCommand{0, ' ', 0, 'W', ' ', ' ', false, ParsingStages::Finish}},
			  Param{10, 0, 0, 1, NormalModeCommand{0, ' ', 0, 'W', ' ', ' ', false, ParsingStages::Finish}},
			  Param{0, 1, 6, 1, NormalModeCommand{0, ' ', 0, 'W', ' ', ' ', false, ParsingStages::Finish}},
			  Param{6, 1, 12, 1, NormalModeCommand{0, ' ', 0, 'W', ' ', ' ', false, ParsingStages::Finish}},
			  Param{12, 1, 0, 2, NormalModeCommand{0, ' ', 0, 'W', ' ', ' ', false, ParsingStages::Finish}},
			  Param{0, 2, 0, 2, NormalModeCommand{0, ' ', 0, 'W', ' ', ' ', false, ParsingStages::Finish}}));

class Word : public ::testing::Test {
	  protected:
	NormalMode normalMode;
	std::unique_ptr<ITextBuffer> matrix;
	const std::filesystem::path inputPath = "temp";
	Document doc;
	EditorState state{};

	Word() : doc{std::move(matrix), inputPath} {

		matrix = std::make_unique<Matrix>();
		doc = Document(std::move(matrix), inputPath);
		doc.cursor_.setX(0);
		doc.cursor_.setY(0);

		doc.textBuffer_->insertLine(0, " ...Hello World ...");
		doc.textBuffer_->insertLine(1, " Hello");
	}

	void SetUp() override {
		matrix = std::make_unique<Matrix>();
		doc = Document(std::move(matrix), inputPath);
		doc.cursor_.setX(0);
		doc.cursor_.setY(0);

		doc.textBuffer_->insertLine(0, " ...Hello World ...");
		doc.textBuffer_->insertLine(1, " Hello");
	}
};

class WordTests : public Word, public testing::WithParamInterface<Param> {};

TEST_P(WordTests, HandleLotsOfCases) {
	const auto& [curX, curY, x, y, command] = GetParam();

	this->doc.cursor_.setX(curX);
	this->doc.cursor_.setY(curY);

	const NormalModeTable table{};
	NormalModeExecutor executor{table};

	executor.executeNormalModeCommand(doc.textBuffer_, doc.cursor_, state, command);

	ASSERT_EQ(doc.cursor_.getX(), x);
	ASSERT_EQ(doc.cursor_.getY(), y);
}

INSTANTIATE_TEST_SUITE_P(
	WordMotionCommands, WordTests,
	::testing::Values(Param{0, 0, 1, 0, NormalModeCommand{0, ' ', 0, 'w', ' ', ' ', false, ParsingStages::Finish}},
			  Param{1, 0, 4, 0, NormalModeCommand{0, ' ', 0, 'w', ' ', ' ', false, ParsingStages::Finish}},
			  Param{4, 0, 10, 0, NormalModeCommand{0, ' ', 0, 'w', ' ', ' ', false, ParsingStages::Finish}},
			  Param{10, 0, 16, 0, NormalModeCommand{0, ' ', 0, 'w', ' ', ' ', false, ParsingStages::Finish}},
			  Param{16, 0, 1, 1, NormalModeCommand{0, ' ', 0, 'w', ' ', ' ', false, ParsingStages::Finish}},
			  Param{1, 1, 0, 2, NormalModeCommand{0, ' ', 0, 'w', ' ', ' ', false, ParsingStages::Finish}}));

INSTANTIATE_TEST_SUITE_P(
	word_back_and_end, WordTests,
	::testing::Values(Param{4, 0, 1, 0, NormalModeCommand{0, ' ', 0, 'b', ' ', ' ', false, ParsingStages::Finish}},
			  Param{10, 0, 4, 0, NormalModeCommand{0, ' ', 0, 'b', ' ', ' ', false, ParsingStages::Finish}},
			  Param{1, 1, 16, 0, NormalModeCommand{0, ' ', 0, 'b', ' ', ' ', false, ParsingStages::Finish}},
			  Param{0, 0, 3, 0, NormalModeCommand{0, ' ', 0, 'e', ' ', ' ', false, ParsingStages::Finish}},
			  Param{3, 0, 8, 0, NormalModeCommand{0, ' ', 0, 'e', ' ', ' ', false, ParsingStages::Finish}},
			  Param{8, 0, 14, 0, NormalModeCommand{0, ' ', 0, 'e', ' ', ' ', false, ParsingStages::Finish}}));

INSTANTIATE_TEST_SUITE_P(
	WORD_Back_And_End, NormalModeMotionsTest,
	::testing::Values(Param{10, 0, 1, 0, NormalModeCommand{0, ' ', 0, 'B', ' ', ' ', false, ParsingStages::Finish}},
			  Param{0, 1, 10, 0, NormalModeCommand{0, ' ', 0, 'B', ' ', ' ', false, ParsingStages::Finish}},
			  Param{12, 1, 6, 1, NormalModeCommand{0, ' ', 0, 'B', ' ', ' ', false, ParsingStages::Finish}},
			  Param{0, 0, 8, 0, NormalModeCommand{0, ' ', 0, 'E', ' ', ' ', false, ParsingStages::Finish}},
			  Param{8, 0, 15, 0, NormalModeCommand{0, ' ', 0, 'E', ' ', ' ', false, ParsingStages::Finish}},
			  Param{15, 0, 4, 1, NormalModeCommand{0, ' ', 0, 'E', ' ', ' ', false, ParsingStages::Finish}}));*/