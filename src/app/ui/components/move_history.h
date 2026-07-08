#pragma once
#include <string>
#include <vector>
#include "raylib.h"
#include "app/ui/theme.h"

namespace ui {

struct HistoryRow {
    int moveNumber{0};
    std::string white;   // white's move (or game mode for row 0)
    std::string black;   // black's move (empty if not yet played)
};

struct MoveHistory {
    std::vector<HistoryRow> rows;
    int hoveredIdx = -1;
    int hoveredCell = -1; // 0 = left (white), 1 = right (black)

    void draw(int x, int y, int w, const Theme& t);
    int handleInput(Vector2 mouse, bool click);
    int totalHeight(const Theme& t) const;
};

} // namespace ui
