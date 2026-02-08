#include "utils/Config.hpp"

#include <commands/NormalMode.hpp>
#include <core/Editor.hpp>
#include <gtest/gtest.h>

constexpr char lineChar = static_cast<char>(128);

struct ParseParameters {
	std::string input;
	NormalModeCommand command;
};

class NormalModeCommandParsingTest : public testing::TestWithParam<ParseParameters> {
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
	const NormalModeTable table;
	NormalModeParser parser{table};

	for (const auto i : input) {
		editor.editorState_.input_.push_back(i);
		parser.parseCommand(editor.editorState_.input_);
	}

	const auto com = parser.getCommand();

	ASSERT_EQ(com.count1, command.count1);
	ASSERT_EQ(com.operation, command.operation);
	ASSERT_EQ(com.count2, command.count2);
	ASSERT_EQ(com.motion, command.motion);
	ASSERT_EQ(com.textObject, command.textObject);
	ASSERT_EQ(com.targetChar, command.targetChar);
	ASSERT_EQ(com.ignoreCount, command.ignoreCount);
	ASSERT_EQ(com.stage, command.stage);
}

INSTANTIATE_TEST_SUITE_P(
    RegularCommandParsingTest, NormalModeCommandParsingTest,
    testing::Values(
	ParseParameters{"l", NormalModeCommand{0, ' ', 0, 'l', ' ', ' ', false, ParsingStages::Finish}},
	ParseParameters{"32l", NormalModeCommand{32, ' ', 0, 'l', ' ', ' ', false, ParsingStages::Finish}},
	ParseParameters{"dl", NormalModeCommand{0, 'd', 0, 'l', ' ', ' ', false, ParsingStages::Finish}},
	ParseParameters{"32dl", NormalModeCommand{32, 'd', 0, 'l', ' ', ' ', false, ParsingStages::Finish}},
	ParseParameters{"d21l", NormalModeCommand{0, 'd', 21, 'l', ' ', ' ', false, ParsingStages::Finish}},
	ParseParameters{"30d21l", NormalModeCommand{30, 'd', 21, 'l', ' ', ' ', false, ParsingStages::Finish}}));

INSTANTIATE_TEST_SUITE_P(
    ComplexeCommandParsing, NormalModeCommandParsingTest,
    testing::Values(
	ParseParameters{"0", NormalModeCommand{0, ' ', 0, '0', ' ', ' ', false, ParsingStages::Finish}},
	ParseParameters{"d0", NormalModeCommand{0, 'd', 0, '0', ' ', ' ', false, ParsingStages::Finish}},
	ParseParameters{"32d0", NormalModeCommand{32, 'd', 0, '0', ' ', ' ', false, ParsingStages::Finish}},
	ParseParameters{"a", NormalModeCommand{0, 'a', 0, ' ', ' ', ' ', false, ParsingStages::Finish}},
	ParseParameters{"32a", NormalModeCommand{32, 'a', 0, ' ', ' ', ' ', false, ParsingStages::Finish}},
	ParseParameters{"r", NormalModeCommand{0, ' ', 0, ' ', 'r', ' ', true, ParsingStages::WaitingForTargetChar}},
	ParseParameters{"rp", NormalModeCommand{0, ' ', 0, ' ', 'r', 'p', true, ParsingStages::Finish}},
	ParseParameters{"32rp", NormalModeCommand{32, ' ', 0, ' ', 'r', 'p', true, ParsingStages::Finish}},
	ParseParameters{"dd",NormalModeCommand{0, 'd', 0, lineChar, ' ', ' ', false, ParsingStages::Finish}},
	ParseParameters{"32dd",NormalModeCommand{32, 'd', 0, lineChar, ' ', ' ', false, ParsingStages::Finish}}));

INSTANTIATE_TEST_SUITE_P(
    TextObjectParsingTest, NormalModeCommandParsingTest,
    testing::Values(
	ParseParameters{"fp", NormalModeCommand{0, ' ', 0, ' ', 'f', 'p', false, ParsingStages::Finish}},
	ParseParameters{"32fp", NormalModeCommand{32, ' ', 0, ' ', 'f', 'p', false, ParsingStages::Finish}},
	ParseParameters{"dfp", NormalModeCommand{0, 'd', 0, ' ', 'f', 'p', false, ParsingStages::Finish}},
	ParseParameters{"32dfp", NormalModeCommand{32, 'd', 0, ' ', 'f', 'p', false, ParsingStages::Finish}},
	ParseParameters{"d20fp", NormalModeCommand{0, 'd', 20, ' ', 'f', 'p', false, ParsingStages::Finish}},
	ParseParameters{"32d20fp", NormalModeCommand{32, 'd', 20, ' ', 'f', 'p', false, ParsingStages::Finish}}));

INSTANTIATE_TEST_SUITE_P(
    CommandParsingStageTest, NormalModeCommandParsingTest,
    testing::Values(
	ParseParameters{
	    "3", NormalModeCommand{3, ' ', 0, ' ', ' ', ' ', false, ParsingStages::Count1OperationMotionTextObject}},
	ParseParameters{"d", NormalModeCommand{0, 'd', 0, ' ', ' ', ' ', false, ParsingStages::Count2MotionTextObject}},
	ParseParameters{"32d",
			NormalModeCommand{32, 'd', 0, ' ', ' ', ' ', false, ParsingStages::Count2MotionTextObject}},
	ParseParameters{"32d2",
			NormalModeCommand{32, 'd', 2, ' ', ' ', ' ', false, ParsingStages::Count2MotionTextObject}},
	ParseParameters{"32d20f",NormalModeCommand{32, 'd', 20, ' ', 'f', ' ', false, ParsingStages::WaitingForTargetChar}}));
