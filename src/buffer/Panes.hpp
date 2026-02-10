#pragma once
#include <cstdint>
#include <vector>

using FileId = uint_fast64_t;

struct PaneView final {
    int startX;
    int startY;

    int endX_;
    int endY_;
};

struct TextBufferView final {
    int startY_;
    int startX_;

    int visibleLines_;
    int visibleColumns_;
};

class Pane final {
public:
    Pane() = delete;
    Pane(PaneView, TextBufferView, FileId);
    ~Pane() noexcept = default;

    PaneView paneView_;
    TextBufferView textView_;

    FileId fileId_;
};

class Panes final {
public:
    Panes();
    ~Panes() noexcept = default;

    void addPane(PaneView, TextBufferView, FileId);
    void removePane(FileId);

    FileId activeFileId_;
    std::vector<Pane> panes_;
};
