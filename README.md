# Text Editor-V

## Features Overview

### 1. Normal Mode Operations
In Normal Mode, users can perform a variety of operations efficiently. Users can navigate and manipulate text using several commands designed for quick access and extensive functionality.

- **Navigating Text:** Use `h`, `j`, `k`, `l` for left, down, up, and right navigation.
- **Editing Text:** Operations such as `x` to delete a character, `dd` to delete a line, and `yy` to copy a line.

### 2. Motions
The Editor supports various motions to make cursor navigation seamless:

- **Word Motions:** `w` to jump to the start of the next word and `b` to go back to the beginning of the previous word.
- **Screen Motions:** Use `Ctrl + f` to scroll forward a page, and `Ctrl + b` to scroll backward.

### 3. Text Objects
Text objects allow users to select and manipulate blocks of text easily:

- **Selecting Words:** Use `iw` to select the word under the cursor, and `a` for around the word including spaces.
- **Selecting Paragraphs:** Use `ip` for inner paragraph and `ap` for around paragraph.

### 4. Insert Mode Capabilities
Insert Mode is where text is actually inserted into the document. Features include:

- **Basic Insertion:** Press `i` to enter Insert Mode and type freely.
- **Append Text:** Use `a` to append text right after the cursor's position.

### 5. Unit Tests
To ensure features work as intended, comprehensive unit tests are included. These tests cover:

- Normal mode operations to check edit and navigation commands.
- Insert mode functionalities to verify text entry and command execution.

## Conclusion
This enhancement of Text Editor-V provides a structured way to understand its features. With its Normal and Insert modes along with robust testing capabilities, users can expect a powerful text editing experience.