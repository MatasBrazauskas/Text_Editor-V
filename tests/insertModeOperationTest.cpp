#include "utils/Config.hpp"

#include <buffer/Matrix.hpp>
#include <commands/InsertMode.hpp>
#include <core/Editor.hpp>

#include <gtest/gtest.h>

constexpr char enterKey = static_cast<char>(130);
constexpr char backSpaceKey = static_cast<char>(129);

struct Params {
	int curX, curY;
	int x, y;
    char key;
    std::vector<std::string> matrixOutput;
};

class InsertModeTest : public testing::Test {
      protected:
	InsertMode insertMode;
	std::unique_ptr<ITextBuffer> matrix;
	const std::filesystem::path inputPath{};
	Document doc;
	EditorState state{};


    InsertModeTest()
        : matrix(std::make_unique<Matrix>()), doc(std::move(matrix), inputPath)
    {
        doc.textBuffer_->insertLine(0, "Line 1");
    }

	void SetUp() override {
		matrix = std::make_unique<Matrix>();
		doc = Document(std::move(matrix), inputPath);
		doc.cursor_.setX(0);
		doc.cursor_.setY(0);

		doc.textBuffer_->insertLine(0, "");
		doc.textBuffer_->insertLine(1, "12345");
		doc.textBuffer_->insertLine(2, "6789");
		doc.textBuffer_->insertLine(3, "");
	}
};

class InsertModeTestWithParams : public InsertModeTest, public testing::WithParamInterface<Params> {};

TEST_P(InsertModeTestWithParams, AllCases) {
	const auto& [curX, curY, x, y, key, matrixOutput] = GetParam();

    doc.cursor_.setX(curX);
    doc.cursor_.setY(curY);

    state.input_.push_back(key);
    insertMode.HandleKeyboardInput(state, doc);

    ASSERT_EQ(doc.cursor_.getX(), x);
    ASSERT_EQ(doc.cursor_.getY(), y);

    for (auto i{0}; i < matrixOutput.size(); i++) {
        ASSERT_EQ(matrixOutput.at(i), std::string{doc.textBuffer_->rowsView(i)});
    }
}

INSTANTIATE_TEST_SUITE_P(
    backSpaceKeyOperations, InsertModeTestWithParams,
    testing::Values(
        Params{0,0,0,0, backSpaceKey, {"", "12345", "6789", ""}},
        Params{1,1,0,1, backSpaceKey, {"", "2345", "6789", ""}},
        Params{0,1,0,0, backSpaceKey, {"12345", "6789", ""}}
        ));

INSTANTIATE_TEST_SUITE_P(
    insertKeyOperations, InsertModeTestWithParams,
    testing::Values(
        Params{1,1,0,2, enterKey, {"", "1", "2345", "6789", ""}},
        Params{0,0,0,1, enterKey, {"", "", "12345", "6789", ""}},
        Params{0,3,0,4, enterKey, {"", "12345", "6789", "", ""}}
        ));