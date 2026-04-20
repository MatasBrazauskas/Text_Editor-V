# Text Editor-V

A lightweight, vim-inspired text editor built in C++ with SDL2 rendering. Features a powerful command parsing system with multiple editing modes optimized for efficient text manipulation.

## 🎯 Features Overview

### 📍 Normal Mode
The primary editing mode featuring vim-like command parsing and execution.

#### Basic Navigation
- `h`, `j`, `k`, `l` - Move cursor left, down, up, right
- `w` - Jump to the start of the next word
- `W` - Jump to the start of the next WORD (space-separated)
- `b` - Jump back to the start of the previous word
- `B` - Jump back to the start of the previous WORD
- `e` - Jump to the end of the current word
- `E` - Jump to the end of the current WORD
- `0` - Move to the beginning of the line
- `^` - Move to the first non-whitespace character of the line
- `$` - Move to the end of the line
- `G` - Jump to the end of the file

#### Editing Operations
- `x` - Delete the character under the cursor
- `d` + motion - Delete text specified by motion (e.g., `dw` deletes a word, `d$` deletes to end of line)
- `y` + motion - Copy (yank) text specified by motion
- `r` + char - Replace the character under the cursor with the specified character
- `f` + char - Find the specified character to the right on the current line
- `F` + char - Find the specified character to the left on the current line

#### Text Insertion & Manipulation
- `i` - Enter Insert Mode at the current cursor position
- `a` - Enter Insert Mode after the current cursor position
- `O` - Insert a new line above and enter Insert Mode
- `o` - Insert a new line below and enter Insert Mode
- `:` - Enter Command Mode

#### Command Counts
All motions and operations support numeric prefixes for repeated execution:
- `3w` - Jump 3 words forward
- `2dj` - Delete 2 lines down
- `5x` - Delete 5 characters

### Window & File Management

#### Window Mode (`Ctrl + w`)
Split and navigate multiple editing panes:
- `h`, `j`, `k`, `l` - Navigate between panes
- `v` - Split window vertically
- `s` - Split window horizontally
- `c` - Close current pane
- `=` - Equalize pane sizes

#### File Mode (`Ctrl + f`)
Open and browse files:
- `j`, `k` - Navigate file list
- `Enter` - Open selected file
- `u` - Go to parent directory
- `r` - Refresh file listing

### ✏️ Insert Mode
Full text entry with smart indentation and special key handling:
- Type text normally
- `Enter` - Create a new line (preserves remaining line content)
- `Backspace` - Delete the previous character (or merge with previous line)
- `Tab` - Insert configurable indentation (spaces or tabs)

### ⚙️ Command Mode (`:`)
Execute file and buffer operations:
- `:w` - Write (save) current file
- `:w filename` - Save current file as filename
- `:e filename` - Open filename
- `:q` - Quit the editor
- `:bn` - Switch to next buffer
- `:bp` - Switch to previous buffer

### 🎨 Configuration
The editor is highly configurable via `config.json`:
- **Window**: Title, dimensions, FPS limit
- **Editor Feel**: Indent type/size, auto-save interval, cursor blink rate, text wrapping
- **View**: Vertical ruler, line number mode (relative/absolute)
- **Fonts**: Separate fonts for code and UI
- **Theme**: Full color customization (background, foreground, UI, cursor, highlights)

### 🧪 Testing
Comprehensive unit tests cover:
- Normal Mode command parsing (counts, operations, motions, combinations)
- Normal Mode motion accuracy
- Insert Mode operations (text entry, special keys)

## 🏗️ Architecture

The editor is built around a sophisticated **command parsing pipeline**:

1. **Parser** - Tokenizes input into counts, operations, and motions
2. **Executor** - Executes parsed commands with calculated text ranges
3. **Mode System** - Cleanly separated Normal, Insert, and Command modes
4. **Buffer Management** - Multi-pane, multi-file editing with undo/redo stacks
5. **Renderer** - SDL2-based rendering with configurable themes

## 🚀 Getting Started

### Build Requirements
- C++20
- SDL2
- CMake 3.20+

### Building
```bash
mkdir build
cd build
cmake ..
make
```

### Running
```bash
./V [filename]
```

Open a file, navigate with hjkl, enter Insert Mode with `i`, and type away!