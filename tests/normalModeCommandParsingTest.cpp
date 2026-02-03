#include "utils/Config.hpp"

#include <buffer/Matrix.hpp>
#include <commands/NormalMode.hpp>
#include <core/Editor.hpp>
#include <gtest/gtest.h>

struct ParseParameters {
	std::string input;
	NormalModeCommand command;
};

class NormalModeCommandParsingTest : public ::testing::TestWithParam<ParseParameters> {
      protected:
	FileHandler fileHandler;
	Files files;
	Editor editor;
	EditorState editorState;
	NormalMode normalMode;

	NormalModeCommandParsingTest() : files(fileHandler, 0, nullptr), editor(files, fileHandler, editorState) {}
};

TEST_P(NormalModeCommandParsingTest, HandleLotsOfCases) {
	const auto& [input, command] = GetParam();

	auto matrix = std::make_unique<Matrix>();
	const std::filesystem::path inputPath = "temp";
	auto doc = Document(std::move(matrix), inputPath);

	for (char i : input) {
		editor.editorState_.input_.push_back(i);
		normalMode.parseCommand(editor.editorState_.input_, editor.editorState_.input_.back());
	}

	ASSERT_EQ(normalMode.command.count1, command.count1);
	ASSERT_EQ(normalMode.command.operation, command.operation);
	ASSERT_EQ(normalMode.command.count2, command.count2);
	ASSERT_EQ(normalMode.command.motion, command.motion);
	ASSERT_EQ(normalMode.command.textObject, command.textObject);
	ASSERT_EQ(normalMode.command.targetChar, command.targetChar);
	ASSERT_EQ(normalMode.command.ignoreCount, command.ignoreCount);
	ASSERT_EQ(normalMode.command.stage, command.stage);
}

INSTANTIATE_TEST_SUITE_P(
    CommandParsingTest, NormalModeCommandParsingTest,
    ::testing::Values(
	ParseParameters{"a", NormalModeCommand{0, 'a', 0, ' ', ' ', ' ', false, ParsingStages::Finish}},
	ParseParameters{"32a", NormalModeCommand{32, 'a', 0, ' ', ' ', ' ', false, ParsingStages::Finish}},
	ParseParameters{"l", NormalModeCommand{0, ' ', 0, 'l', ' ', ' ', false, ParsingStages::Finish}},
	ParseParameters{"32l", NormalModeCommand{32, ' ', 0, 'l', ' ', ' ', false, ParsingStages::Finish}},
	ParseParameters{"dl", NormalModeCommand{0, 'd', 0, 'l', ' ', ' ', false, ParsingStages::Finish}},
	ParseParameters{"32dl", NormalModeCommand{32, 'd', 0, 'l', ' ', ' ', false, ParsingStages::Finish}},
	ParseParameters{"d21l", NormalModeCommand{0, 'd', 21, 'l', ' ', ' ', false, ParsingStages::Finish}},
	ParseParameters{"30d21l", NormalModeCommand{30, 'd', 21, 'l', ' ', ' ', false, ParsingStages::Finish}},
	ParseParameters{"0", NormalModeCommand{0, ' ', 0, '0', ' ', ' ', false, ParsingStages::Finish}},
	ParseParameters{"32d0", NormalModeCommand{32, 'd', 0, '0', ' ', ' ', false, ParsingStages::Finish}}));

INSTANTIATE_TEST_SUITE_P(
    TextObjectParsingTest, NormalModeCommandParsingTest,
    ::testing::Values(
	ParseParameters{"fp", NormalModeCommand{0, ' ', 0, ' ', 'f', 'p', false, ParsingStages::Finish}},
	ParseParameters{"32fp", NormalModeCommand{32, ' ', 0, ' ', 'f', 'p', false, ParsingStages::Finish}},
	ParseParameters{"dfp", NormalModeCommand{0, 'd', 0, ' ', 'f', 'p', false, ParsingStages::Finish}},
	ParseParameters{"32dfp", NormalModeCommand{32, 'd', 0, ' ', 'f', 'p', false, ParsingStages::Finish}},
	ParseParameters{"d20fp", NormalModeCommand{0, 'd', 20, ' ', 'f', 'p', false, ParsingStages::Finish}},
	ParseParameters{"32d20fp", NormalModeCommand{32, 'd', 20, ' ', 'f', 'p', false, ParsingStages::Finish}}));

INSTANTIATE_TEST_SUITE_P(
    CommandStageTest, NormalModeCommandParsingTest,
    ::testing::Values(
	ParseParameters{
	    "3", NormalModeCommand{3, ' ', 0, ' ', ' ', ' ', false, ParsingStages::Count1OperationMotionTextObject}},
	ParseParameters{"d", NormalModeCommand{0, 'd', 0, ' ', ' ', ' ', false, ParsingStages::Count2MotionTextObject}},
	ParseParameters{"32d",
			NormalModeCommand{32, 'd', 0, ' ', ' ', ' ', false, ParsingStages::Count2MotionTextObject}},
	ParseParameters{"32d2",
			NormalModeCommand{32, 'd', 2, ' ', ' ', ' ', false, ParsingStages::Count2MotionTextObject}},
	ParseParameters{"32d20f",
			NormalModeCommand{32, 'd', 20, ' ', 'f', ' ', false, ParsingStages::WaitingForTargetChar}}));