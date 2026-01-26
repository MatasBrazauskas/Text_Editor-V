#pragma once

#include <format>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

class EditorState;
class Document;
class ITextBuffer;
class TextBufferView;
class Cursor;

using namespace std::string_view_literals;

#define FUNC_TYPES std::unique_ptr<ITextBuffer>&text, Cursor &cursor, EditorState &state

struct MotionRange {
    int startX, startY;
    int endX, endY;
};

enum class ParsingStages: char {
    Count1OperationMotionTextObject,
    Count2MotionTextObject,
    TextObject,
    Finish,
};

class NormalModeCommand {
public:
    NormalModeCommand();
    ~NormalModeCommand() noexcept = default;

    int count1;
    std::string operation;
    int count2;
    std::string motion;
    std::string textObject;
    ParsingStages stage;
};

class NormalMode final {
public:
	NormalMode();
	void HandleKeyboardInput(EditorState&, std::reference_wrapper<Document>);
	using Func = void (NormalMode::*)(FUNC_TYPES);

private:
    std::unordered_map<std::string, Func> operations;
    std::unordered_map<std::string, Func> motions;
    std::unordered_map<std::string, Func> textObjects;

    bool parseOperation(NormalModeCommand& com, const char inputChar);
    bool parseMotion(NormalModeCommand& com, const char inputChar);
    bool parseTextObject(NormalModeCommand& com, const char inputChar);

	Func paramFunc_;
	size_t paramCount_;

    NormalModeCommand command;

    void moveCursorLeft(FUNC_TYPES);
    void moveCursorDown(FUNC_TYPES);
    void moveCursorUp(FUNC_TYPES);
    void moveCursorRight(FUNC_TYPES);

    void moveCursorTopFile(FUNC_TYPES);
    void moveCursorBottomFile(FUNC_TYPES);

    void moveRightMost(FUNC_TYPES);
    void moveLeftMostChar(FUNC_TYPES);
    void moveLeftMost(FUNC_TYPES);

    void deleteLine(FUNC_TYPES);
    void deleteChar(FUNC_TYPES);
    void deleteWord(FUNC_TYPES);
    void deleteAllWord(FUNC_TYPES);

    void findFirstCharRight(FUNC_TYPES);
    void findFirstCharLeft(FUNC_TYPES);

    void insertLineAbove(FUNC_TYPES);
    void insertLineBelow(FUNC_TYPES);

    void switchToInsertLeft(FUNC_TYPES);
    void switchToInsertRight(FUNC_TYPES);

    void currentLine(FUNC_TYPES);

    void updateView(TextBufferView&, const Cursor&);
};