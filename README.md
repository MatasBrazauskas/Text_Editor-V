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

<figure>
  <img width="1864" height="1107" alt="image" src="https://github.com/user-attachments/assets/43ea7369-b759-4eee-8f82-f68dfafac633" />
  <figcaption>Screenshot 1: Normal Mode</figcaption>
</figure>

<figure>
  <img width="1864" height="1107" alt="image" src="https://github.com/user-attachments/assets/85c2dd2b-8e81-4b7c-aee2-708e97ff9947" />
  <figcaption>Screenshot 2: Insert Mode</figcaption>
</figure>

<figure>
  <img width="1864" height="1107" alt="image" src="https://github.com/user-attachments/assets/54e86629-2b99-4f2d-8760-9b1601223333" />
  <figcaption>Screenshot 3: Command Mode</figcaption>
</figure>

<figure>
  <img width="1864" height="1107" alt="image" src="https://github.com/user-attachments/assets/a59c0b17-c1e0-4d65-b4e4-3581c6ede9f7" />
  <figcaption>Screenshot 4: Window mode with panes and splits</figcaption>
</figure>

<figure>
  <img width="1864" height="1107" alt="image" src="https://github.com/user-attachments/assets/d5d4ee64-148e-45d0-9bd3-694d17178adc" />
  <figcaption>Screenshot 5: File mode</figcaption>
</figure>

<figure>
  <img width="1864" height="1107" alt="image" src="https://github.com/user-attachments/assets/08bb504b-6617-407f-930c-83377d371751" />
  <figcaption>Screenshot 6: Multiple files open: main.cpp and config.json</figcaption>
</figure>






