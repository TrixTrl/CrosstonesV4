#pragma once
#include "raylib.h"
#include "data/GamePosition.h"
#include "data/MoveInfo.h"

namespace ui {

class Board {
public:
    void setPosition(const GamePosition& pos);
    void setHighlights(const bool (&hl)[13][13]);
    void setMoveInfo(const MoveInfo* info);
    void clearHighlights();
    void setHistoryPreview(const GamePosition& state);
    void clearHistoryPreview();
    void draw(Rectangle rect, float uiScale = 1.0f);

    GamePosition& position() { return displayBoard; }
    const GamePosition& position() const { return displayBoard; }

private:
    GamePosition displayBoard{};
    bool highlights[13][13]{};
    const MoveInfo* moveInfo = nullptr;

    bool hasHistoryPreview = false;
    GamePosition historyPreviewState{};

    void drawGrid(int cellSize, int ox, int oy, const GamePosition& state);
    void drawMovePath(int cellSize, int ox, int oy);
    void drawPieces(const GamePosition& state, int cellSize, int ox, int oy);
    void drawHighlights(const bool hl[13][13], int cellSize, int ox, int oy);
};

} // namespace ui
