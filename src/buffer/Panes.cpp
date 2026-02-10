#include "Panes.hpp"

#include <algorithm>

Pane::Pane(const PaneView paneView_t, const TextBufferView textView_t, const FileId fileId_t)
    : paneView_{paneView_t}, textView_{textView_t}, fileId_ {fileId_t} {}

Panes::Panes(): activeFileId_{} {}

void Panes::addPane(const PaneView paneView_t, const TextBufferView textView_t, const FileId fileId_t) {
    panes_.emplace_back(paneView_t, textView_t, fileId_t);
}

void Panes::removePane(const FileId fileId_t) {
    const auto predicate = [fileId_t](const Pane& pane){return pane.fileId_ == fileId_t; };
    const auto it = std::remove_if(panes_.begin(), panes_.end(), predicate);
    panes_.erase(it, panes_.end());
}

