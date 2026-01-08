#include "commands/InsertMode.hpp"

#include <iostream>

#include "core/Editor.hpp"

//Dont forget to update that shit ok (errors and checks)
InsertMode::InsertMode() {
    const auto b = std::string(1, static_cast<char>(SpecialKeys::Enter));
    const auto c = std::string(1, static_cast<char>(SpecialKeys::Backspace));

    commands_ = {
        {b, &InsertMode::handleEnter },
        {c, &InsertMode::handleBackspace}
    };
}

void InsertMode::handleEnter(EditorState&, Document& doc) const {
    std::cout << "Enter pressed\n";
    doc.textBuffer_->insertLine(doc.cursor_.getY() + 1);
    doc.cursor_.setY(doc.cursor_.getY() + 1);
}
void InsertMode::handleBackspace(EditorState&, Document& doc) const {
    std::cout << "Backspace pressed\n";
    doc.textBuffer_->deleteCharacter(doc.cursor_.getY(), doc.cursor_.getX() - 1);
    doc.cursor_.decrementX();
}

void InsertMode::HandleKeyboardInput(EditorState& editorState, Document& doc) const {
    if (editorState.input_.empty()) return;

    if (const auto it = commands_.find(editorState.input_); it != commands_.end()) {
        auto func = it->second;
        (this->*func)(editorState, doc);
    } else {
        doc.textBuffer_->insertCharacter(doc.cursor_.getY(), doc.cursor_.getX(), editorState.input_.at(0));
        doc.cursor_.incrementX();
    }

    editorState.input_.clear();
}