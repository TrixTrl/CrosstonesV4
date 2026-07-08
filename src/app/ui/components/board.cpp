#include "board.h"
#include <string>
#include <cstring>
#include <vector>
#include <algorithm>
#include "globals/Piece.h"
#include "app/FontManager.h"

namespace ui {

void Board::setPosition(const GamePosition& pos) {
    displayBoard = pos;
}

void Board::setHighlights(const bool (&hl)[13][13]) {
    std::memcpy(highlights, hl, sizeof(highlights));
}

void Board::setMoveInfo(const MoveInfo* info) {
    moveInfo = info;
}

void Board::clearHighlights() {
    std::memset(highlights, 0, sizeof(highlights));
}

void Board::setHistoryPreview(const GamePosition& state) {
    hasHistoryPreview = true;
    historyPreviewState = state;
}

void Board::clearHistoryPreview() {
    hasHistoryPreview = false;
}

void Board::draw(Rectangle rect, float scale) {
    float borderRatio = 0.7f;
    float totalUnits = 13.0f + 2.0f * borderRatio;
    int cellSize = (std::min)((int)(rect.width / totalUnits), (int)(rect.height / totalUnits));
    if (cellSize < (int)(20 * scale)) cellSize = (int)(20 * scale);

    float borderPx = borderRatio * cellSize;
    int gridW = cellSize * 13;
    int gridH = cellSize * 13;
    int ox = (int)rect.x + ((int)rect.width  - cellSize * 13) / 2;
    int oy = (int)rect.y + ((int)rect.height - cellSize * 13) / 2;

    // Border background
    Color borderColor{122, 70, 58, 255};
    DrawRectangle(ox, oy - (int)borderPx, gridW, (int)borderPx, borderColor);
    DrawRectangle(ox, oy + gridH, gridW, (int)borderPx, borderColor);
    DrawRectangle(ox - (int)borderPx, oy - (int)borderPx, (int)borderPx, gridH + (int)(borderPx * 2), borderColor);
    DrawRectangle(ox + gridW, oy - (int)borderPx, (int)borderPx, gridH + (int)(borderPx * 2), borderColor);

    // Label text
    float labelSize = cellSize * 0.35f;
    Color labelColor{231, 176, 88, 255};

    for (int i = 0; i < 13; i++) {
        char buf[2] = {(char)('A' + i), 0};
        float tw = MeasureAppText(buf, labelSize);
        float cx = ox + i * cellSize + (cellSize - tw) * 0.5f;
        DrawAppText(buf, cx, (float)(oy + gridH + (int)((borderPx - labelSize) * 0.5f)), labelSize, labelColor);
        DrawAppText(buf, cx, (float)(oy - (int)((borderPx + labelSize) * 0.5f)), labelSize, labelColor);
    }

    for (int j = 0; j < 13; j++) {
        std::string num = std::to_string(13 - j);
        float tw = MeasureAppText(num.c_str(), labelSize);
        float cy = oy + j * cellSize + (cellSize - labelSize) * 0.5f;
        DrawAppText(num.c_str(), (float)(ox - (int)((borderPx + tw) * 0.5f)), cy, labelSize, labelColor);
        DrawAppText(num.c_str(), (float)(ox + gridW + (int)((borderPx - tw) * 0.5f)), cy, labelSize, labelColor);
    }

    const GamePosition& gridState = hasHistoryPreview ? historyPreviewState : displayBoard;
    drawGrid(cellSize, ox, oy, gridState);
    drawMovePath(cellSize, ox, oy);

    if (hasHistoryPreview) {
        drawHighlights(highlights, cellSize, ox, oy);
        drawPieces(historyPreviewState, cellSize, ox, oy);
    } else {
        bool previewing = moveInfo && !moveInfo->xmove.empty();
        GamePosition previewState;
        bool diffHighlights[13][13]{};
        if (previewing) {
            previewState = displayBoard;
            for (auto& xm : moveInfo->xmove)
                previewState[xm.i][xm.j] ^= xm.delta;

            for (int i = 0; i < 13; i++)
                for (int j = 0; j < 13; j++)
                    diffHighlights[i][j] = displayBoard[i][j] != previewState[i][j];
        }

        const bool (&finalHighlights)[13][13] = previewing ? diffHighlights : highlights;
        drawHighlights(finalHighlights, cellSize, ox, oy);

        const GamePosition& drawState = previewing ? previewState : displayBoard;
        drawPieces(drawState, cellSize, ox, oy);
    }
}

static const int defaultCellSize = 75;

void Board::drawGrid(int cellSize, int ox, int oy, const GamePosition& state) {
    std::string drawTypes[] = {
        "ahthbht",
        "v v b v",
        "ththbht",
        "v v b v",
        "bbbbbbb",
        "b v v b",
        "ahththa"
    };

    enum DrawPath { NONE = 0, HORIZONTAL, VERTICAL };

    for (int i = 0; i < 13; i++) {
        for (int j = 0; j < 13; j++) {
            Color col;
            DrawPath path = NONE;
            uint8_t piece = state[i][j];
            char typeChar = drawTypes[j < 7 ? j : 5 - (j % 7)][i < 7 ? i : 5 - (i % 7)];

            switch (typeChar) {
                case 'a': col = Color{63, 45, 35, 255}; break;
                case ' ': col = Color{122, 70, 58, 255}; break;
                case 'h': col = Color{214, 176, 116, 255}; path = HORIZONTAL; break;
                case 'v': col = Color{214, 176, 116, 255}; path = VERTICAL; break;
                case 'b': col = Color{231, 176, 88, 255}; break;
                case 't':
                    col = Color{214, 176, 116, 255};
                    path = (piece & Piece::setTurnPiece) == 0 ? HORIZONTAL : VERTICAL;
                    break;
                default: col = Color{255, 0, 0, 255}; break;
            }

            DrawRectangle(ox + i * cellSize, oy + j * cellSize, cellSize, cellSize, col);

            switch (path) {
                case HORIZONTAL:
                    DrawRectangle(ox + i * cellSize, oy + (j + 1.0f/3.0f) * cellSize, cellSize, cellSize / 3, Color{63, 45, 35, 255});
                    break;
                case VERTICAL:
                    DrawRectangle(ox + (i + 1.0f/3.0f) * cellSize, oy + j * cellSize, cellSize / 3, cellSize, Color{63, 45, 35, 255});
                    break;
                default: break;
            }
        }
    }
}

void Board::drawMovePath(int cellSize, int ox, int oy) {
    if (!moveInfo || moveInfo->path.empty()) return;

    struct Pt { int i, j; };
    std::vector<Pt> waypoints;
    if (moveInfo->startI >= 0)
        waypoints.push_back({moveInfo->startI, moveInfo->startJ});
    for (auto& s : moveInfo->path)
        waypoints.push_back({s.i, s.j});

    for (size_t n = 0; n + 1 < waypoints.size(); n++) {
        auto& from = waypoints[n];
        auto& to = waypoints[n + 1];
        Vector2 start = {(float)(ox + from.i * cellSize + cellSize / 2),
                         (float)(oy + from.j * cellSize + cellSize / 2)};
        Vector2 end   = {(float)(ox + to.i   * cellSize + cellSize / 2),
                         (float)(oy + to.j   * cellSize + cellSize / 2)};
        DrawLineEx(start, end, 3.0f, Color{255, 255, 100, 200});
    }
}

void Board::drawPieces(const GamePosition& state, int cellSize, int ox, int oy) {
    Color colors[4][2] = {
        {{15, 15, 15, 255}, {250, 250, 250, 255}},
        {{250, 250, 250, 255}, {15, 15, 15, 255}},
        {{224, 36, 36, 255}, {230, 37, 37, 255}},
        {{31, 20, 250, 255}, {44, 35, 219, 255}}
    };

    for (int i = 0; i < 13; i++) {
        for (int j = 0; j < 13; j++) {
            uint8_t piece = state[i][j];
            if (!Piece::tower(piece)) continue;

            float piecePadding = 0.2f;
            float pieceBorder = 0.1f;
            float textScale = 0.7f;

            int ringColorIndex = 0;
            int centerColorIndex = 0;

            if (Piece::isRed(piece)) ringColorIndex = 2;
            else if (Piece::isBlue(piece)) ringColorIndex = 3;

            if (Piece::isAddOn(piece)) centerColorIndex = ringColorIndex;
            else if (Piece::isWhite(piece)) {
                centerColorIndex = 1;
                if (ringColorIndex == 0) ringColorIndex = 1;
            }

            float cx = (float)(ox + (i + 0.5f) * cellSize);
            float cy = (float)(oy + (j + 0.5f) * cellSize);
            float rx = cellSize / 2.0f * (1 - piecePadding);
            float ry = cellSize / 2.0f * (1 - piecePadding);

            DrawEllipse(cx, cy, rx, ry, colors[ringColorIndex][1]);
            DrawEllipse(cx, cy, rx - cellSize / 2.0f * pieceBorder, ry - cellSize / 2.0f * pieceBorder, colors[centerColorIndex][0]);

            if (Piece::isAddOn(piece)) continue;

            int height = Piece::height(piece);
            std::string hStr = std::to_string(height);
            float textWidth = MeasureAppText(hStr.c_str(), cellSize * textScale);
            DrawAppText(hStr.c_str(), cx - textWidth / 2.0f,
                        (float)(oy + (j + (1 - textScale) / 2.0f) * cellSize),
                        cellSize * textScale, colors[(centerColorIndex + 1) % 2][0]);
        }
    }
}

void Board::drawHighlights(const bool hl[13][13], int cellSize, int ox, int oy) {
    if (!hl) return;

    for (int i = 0; i < 13; i++) {
        for (int j = 0; j < 13; j++) {
            if (hl[i][j]) {
                DrawRectangleLines(ox + i * cellSize, oy + j * cellSize, cellSize, cellSize, YELLOW);
            }
        }
    }
}

} // namespace ui
