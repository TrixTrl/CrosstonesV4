#pragma once
#include <cstdint>
#include <string>
#include "game-suite/fix_win32_compatibility.h"
#include "game-suite/GamePosition.h"
#include "raylib.h"
#include "globals/Piece.h"
#include "FontManager.h"

const int defaultCellSize = 75;

inline void drawBoard(const GamePosition& state, const bool highlights[13][13],
                      int cellSize, int offsetX, int offsetY);

inline void drawBoard(const GamePosition& state, const bool highlights[13][13] = nullptr) {
    drawBoard(state, highlights, defaultCellSize, 0, 0);
}

inline void drawBoard(const GamePosition& state, const bool highlights[13][13],
                      int cellSize, int offsetX, int offsetY) {
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

    ClearBackground(Color{25, 25, 40, 255});

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

            DrawRectangle(offsetX + i * cellSize, offsetY + j * cellSize, cellSize, cellSize, col);

            switch (path) {
                case HORIZONTAL:
                    DrawRectangle(offsetX + i * cellSize, offsetY + (j + 1.0f/3.0f) * cellSize, cellSize, cellSize / 3, Color{63, 45, 35, 255});
                    break;
                case VERTICAL:
                    DrawRectangle(offsetX + (i + 1.0f/3.0f) * cellSize, offsetY + j * cellSize, cellSize / 3, cellSize, Color{63, 45, 35, 255});
                    break;
                default: break;
            }

            if (Piece::tower(piece)) {
                float piecePadding = 0.2f;
                float pieceBorder = 0.1f;
                float textScale = 0.7f;

                Color colors[4][2] = {
                    {{15, 15, 15, 255}, {250, 250, 250, 255}},
                    {{250, 250, 250, 255}, {15, 15, 15, 255}},
                    {{224, 36, 36, 255}, {230, 37, 37, 255}},
                    {{31, 20, 250, 255}, {44, 35, 219, 255}}
                };

                int ringColorIndex = 0;
                int centerColorIndex = 0;

                if (Piece::isRed(piece)) ringColorIndex = 2;
                else if (Piece::isBlue(piece)) ringColorIndex = 3;

                if (Piece::isAddOn(piece)) centerColorIndex = ringColorIndex;
                else if (Piece::isWhite(piece)) {
                    centerColorIndex = 1;
                    if (ringColorIndex == 0) ringColorIndex = 1;
                }

                float cx = offsetX + (i + 0.5f) * cellSize;
                float cy = offsetY + (j + 0.5f) * cellSize;
                float rx = cellSize / 2.0f * (1 - piecePadding);
                float ry = cellSize / 2.0f * (1 - piecePadding);

                DrawEllipse(cx, cy, rx, ry, colors[ringColorIndex][1]);
                DrawEllipse(cx, cy, rx - cellSize / 2.0f * pieceBorder, ry - cellSize / 2.0f * pieceBorder, colors[centerColorIndex][0]);

                if (Piece::isAddOn(piece)) continue;

                int height = Piece::height(piece);
                std::string hStr = std::to_string(height);
                float textWidth = MeasureAppText(hStr.c_str(), cellSize * textScale);
                DrawAppText(hStr.c_str(), cx - textWidth / 2.0f, offsetY + (j + (1 - textScale) / 2.0f) * cellSize, cellSize * textScale, colors[(centerColorIndex + 1) % 2][0]);
            }
        }
    }

    // Second pass: draw highlights on top so no border is overwritten
    if (highlights) {
        for (int i = 0; i < 13; i++) {
            for (int j = 0; j < 13; j++) {
                if (highlights[i][j]) {
                    DrawRectangleLines(offsetX + i * cellSize, offsetY + j * cellSize, cellSize, cellSize, YELLOW);
                }
            }
        }
    }
}
