#include "utils/Config.hpp"

#include <buffer/Matrix.hpp>
#include <commands/NormalMode.hpp>
#include <core/Editor.hpp>
#include <gtest/gtest.h>

struct Param {
    int curX, curY;
    int x, y;
    NormalModeCommand command;
};

class NormalModeMotions : public ::testing::Test {
protected:
    NormalMode normalMode;
    std::unique_ptr<ITextBuffer> matrix;
    const std::filesystem::path inputPath = "temp";
    Document doc;
    EditorState state{};

    NormalModeMotions(): doc{std::move(matrix), inputPath} {}

    void SetUp() override {
        matrix = std::make_unique<Matrix>();
        doc = Document(std::move(matrix), inputPath);
        doc.cursor_.setX(0);
        doc.cursor_.setY(0);

        doc.textBuffer_->insertLine(0, " ...Hello World.");
        doc.textBuffer_->insertLine(1, "Hello World ..a   ");
        doc.textBuffer_->deleteLine(2);
    }
};

class NormalModeMotionsTest : public NormalModeMotions, public testing::WithParamInterface<Param> {};

TEST_P(NormalModeMotionsTest, HandleLotsOfCases) {
    const auto& [curX, curY, x, y, command] = GetParam();

    this->normalMode.command = command;
    this->doc.cursor_.setX(curX);
    this->doc.cursor_.setY(curY);

    this->normalMode.executeNormalModeCommand(doc.textBuffer_, doc.cursor_, state);

    ASSERT_EQ(doc.cursor_.getX(), x);
    ASSERT_EQ(doc.cursor_.getY(), y);
}

TEST_F(NormalModeMotions, idk) {
    matrix = std::make_unique<Matrix>();
    matrix->insertLine(0, "123");
    matrix->insertLine(1, "");

    doc = Document(std::move(matrix), inputPath);
    doc.cursor_.setX(2);
    doc.cursor_.setY(0);

    normalMode.command = NormalModeCommand{0, ' ', 0, 'W', ' ', ' ', false, ParsingStages::Finish};
    normalMode.executeNormalModeCommand(doc.textBuffer_, doc.cursor_, state);

    ASSERT_EQ(doc.cursor_.getX(), 0);
    ASSERT_EQ(doc.cursor_.getY(), 1);
}

INSTANTIATE_TEST_SUITE_P(
    WordMotionCommands,
    NormalModeMotionsTest,
    ::testing::Values(
            Param{0,0,1, 0, NormalModeCommand{0, ' ', 0, 'W', ' ', ' ', false, ParsingStages::Finish}},
            Param{1,0, 10, 0, NormalModeCommand{0, ' ', 0, 'W', ' ', ' ', false, ParsingStages::Finish}},
            Param{10,0, 0, 1, NormalModeCommand{0, ' ', 0, 'W', ' ', ' ', false, ParsingStages::Finish}},
            Param{0,1, 6, 1, NormalModeCommand{0, ' ', 0, 'W', ' ', ' ', false, ParsingStages::Finish}},
            Param{6,1, 12, 1, NormalModeCommand{0, ' ', 0, 'W', ' ', ' ', false, ParsingStages::Finish}},
            Param{12,1, 17, 1, NormalModeCommand{0, ' ', 0, 'W', ' ', ' ', false, ParsingStages::Finish}},
            Param{17,1, 17, 1, NormalModeCommand{0, ' ', 0, 'W', ' ', ' ', false, ParsingStages::Finish}}
            ));