#pragma once
#include <string>
#include <vector>
#include "raylib.h"
#include "app/ui/theme.h"

namespace ui {

struct MoveEntry {
    std::string label;
    std::string subtitle;
    float eval = 0.0f;
    bool isBook = false;
    int originalIndex = -1;
    void* userData = nullptr;
};

struct MoveList {
    std::vector<MoveEntry> entries;
    int hoveredIdx = -1;

    void draw(int x, int y, int w, const Theme& t);
    int handleInput(Vector2 mouse, bool click);
    int totalHeight(const Theme& t) const;
};

} // namespace ui
