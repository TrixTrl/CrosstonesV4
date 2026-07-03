#include "ExploreMode.h"
#include "BoardDraw.h"
#include "FontManager.h"
#include "game-suite/Board.h"
#include "game-suite/GamePosition.h"
#include "globals/Piece.h"
#include <string>
#include <vector>
#include <cstring>

struct PositionPreset {
    const char* name;
    const char* position;
};

static const PositionPreset positionPresets[] = {
    {"Standard Start",
     "b-10000 r-10006 b-10012 -B30500 -B30502 -W30510 -W30512 b-10606 "
     "-B30700 -B30702 -W30710 -W30712 b-11200 r-11206 b-11212 "
     "11111111111111111111"},
    {"Dragon Start",
     "b-10000 b-10012 b-11200 b-11212 b-10606 r-10006 r-11206 -W30512 "
     "-W30510 -B30502 -B30500 -W30712 -W30710 -B30702 -B30700 "
     "11111111111111111111"},
    {"Unsolved Endgame",
     "-B30500 b-11200 bW20204 -W10304 -B30804 -B11204 -B20410 -W11210 "
     "bW20212 r-10412 -W10512 -W20712 11110111110111111010"},
    {"Seagull Gambit",
     "b-10000 b-10012 b-11200 b-11212 rW20006 r-11206 -B10206 -B20204 "
     "-B30500 -B30700 -W30512 -W30712 -W10605 -W10607 -W10808 -W20809 "
     "-B21104 11110111111111111111"},
    {"Win in 4 Ply",
     "-B10700 -W10803 -W10602 00000000000000000000"},
    {"Slaughterhouse",
     "b-10000 r-10006 b-10012 -W30510 -B30502 b-10606 -B30702 -W30710 "
     "b-11200 r-11206 b-11212 11111111111111111111"},
};
static const int numPresets = sizeof(positionPresets) / sizeof(positionPresets[0]);

void ExploreMode::run(const AppConfig& config) {
    const int boardPx = 13 * cellSize;
    const int rightPanelX = boardPx + 20;
    const int rightPanelW = 300;
    const int windowWidth = rightPanelX + rightPanelW + 20;
    const int windowHeight = boardPx;
    const Rectangle dropdownRect = { (float)rightPanelX, 20.0f, (float)rightPanelW, 26.0f };

    InitWindow(windowWidth, windowHeight, "Crosstones V4 - Move Explorer");
    SetTargetFPS(30);
    initAppFont();

    // Find starting preset index
    int presetIdx = 0;
    for (int p = 0; p < numPresets; p++) {
        if (config.explore.position == positionPresets[p].position) {
            presetIdx = p;
            break;
        }
    }

    GamePosition displayBoard[3] = {0};
    Board bs;
    std::bitset<3> set(5);
    bs.rst(set);

    bool isWhite = false;
    bs.loadPosition(positionPresets[presetIdx].position);
    bs.copyPositionTo(displayBoard[0]);

    std::vector<std::vector<Board::xMove>> moves = bs.getMoves(isWhite);

    int i = 0;
    bool paused = false;
    int autoTimer = 0;
    bool dropdownOpen = false;
    int hoveredDropdownIdx = -1;

    while (!WindowShouldClose()) {
        Vector2 mouse = GetMousePosition();
        bool click = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);

        // --- Handle input ---
        bool moveRequested = false;
        int moveDelta = 0;

        if (!dropdownOpen) {
            if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) {
                moveDelta = 1;
                moveRequested = true;
            } else if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) {
                moveDelta = -1;
                moveRequested = true;
            } else if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
                moveDelta = 10;
                moveRequested = true;
            } else if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
                moveDelta = -10;
                moveRequested = true;
            } else if (IsKeyPressed(KEY_DELETE)) {
                i = 0;
                moveRequested = true;
            } else if (IsKeyPressed(KEY_SPACE)) {
                paused = !paused;
                autoTimer = 0;
            }
        }

        // Preset cycling
        bool presetChanged = false;
        if (IsKeyPressed(KEY_LEFT_BRACKET)) {
            presetIdx = (presetIdx - 1 + numPresets) % numPresets;
            presetChanged = true;
        } else if (IsKeyPressed(KEY_RIGHT_BRACKET) || IsKeyPressed(KEY_TAB)) {
            presetIdx = (presetIdx + 1) % numPresets;
            presetChanged = true;
        }

        // Dropdown interaction
        hoveredDropdownIdx = -1;
        if (dropdownOpen) {
            for (int p = 0; p < numPresets; p++) {
                Rectangle optRect = { dropdownRect.x, dropdownRect.y + dropdownRect.height + p * dropdownRect.height, dropdownRect.width, dropdownRect.height };
                if (CheckCollisionPointRec(mouse, optRect)) {
                    hoveredDropdownIdx = p;
                    if (click) {
                        presetIdx = p;
                        presetChanged = true;
                        dropdownOpen = false;
                    }
                    break;
                }
            }
            if (click && hoveredDropdownIdx == -1 && !CheckCollisionPointRec(mouse, dropdownRect)) {
                dropdownOpen = false;
            }
        } else {
            if (click && CheckCollisionPointRec(mouse, dropdownRect)) {
                dropdownOpen = true;
            }
        }

        if (presetChanged) {
            bs = Board();
            bs.rst(set);
            bs.loadPosition(positionPresets[presetIdx].position);
            bs.copyPositionTo(displayBoard[0]);
            moves = bs.getMoves(isWhite);
            i = 0;
            paused = false;
            autoTimer = 0;
        }

        // Auto-advance when not paused and dropdown is closed
        if (!paused && !dropdownOpen && moves.size() > 0) {
            autoTimer++;
            if (autoTimer >= 30) { // ~1 second at 30 FPS
                autoTimer = 0;
                moveDelta = 1;
                moveRequested = true;
            }
        }

        // --- Process move ---
        if (moves.size() > 0) {
            if (moveRequested) {
                i += moveDelta;
                i = (i % (int)moves.size() + (int)moves.size()) % (int)moves.size();
            }
            bs.copyPositionTo(displayBoard[0]);
            Board tempBoard = bs;
            tempBoard.unsafeApplyMove(displayBoard[0], moves[i]);
        } else {
            bs.copyPositionTo(displayBoard[0]);
        }

        // --- Build highlights ---
        bool highlights[13][13] = {false};
        if (moves.size() > 0) {
            for (const auto& xm : moves[i]) {
                if (xm.i >= 0 && xm.i < 13 && xm.j >= 0 && xm.j < 13)
                    highlights[xm.i][xm.j] = true;
            }
        }

        // --- Draw ---
        BeginDrawing();
        drawBoard(&displayBoard[0].pieces, highlights);

        // Right panel background
        DrawRectangle(rightPanelX, 0, windowWidth - rightPanelX, windowHeight, Color{25, 25, 40, 255});
        DrawLine(rightPanelX - 1, 0, rightPanelX - 1, windowHeight, Color{100, 100, 130, 255});

        // Move info
        std::string moveInfo = "Move: " + std::to_string(i + 1) + " / " + std::to_string(moves.size());
        if (paused) moveInfo += "  [PAUSED]";
        DrawAppText(moveInfo.c_str(), (float)rightPanelX, 56.0f, 16, WHITE);

        // Dropdown base button
        DrawRectangleRec(dropdownRect, dropdownOpen ? Color{70, 70, 100, 255} : Color{50, 50, 75, 255});
        DrawRectangleLinesEx(dropdownRect, 1, Color{100, 100, 130, 255});
        std::string presetName = positionPresets[presetIdx].name;
        float nameW = MeasureAppText(presetName.c_str(), 16);
        DrawAppText(presetName.c_str(), dropdownRect.x + dropdownRect.width / 2.0f - nameW / 2.0f, dropdownRect.y + 5, 16, WHITE);
        DrawAppText(dropdownOpen ? "^" : "v", dropdownRect.x + dropdownRect.width - 20, dropdownRect.y + 5, 16, WHITE);

        // Controls
        int ctrlY = 86;
        DrawAppText("SPACE: pause/resume", (float)rightPanelX, (float)ctrlY, 14, Color{180, 180, 180, 255});
        DrawAppText("A/D: navigate", (float)rightPanelX, (float)(ctrlY + 18), 14, Color{180, 180, 180, 255});
        DrawAppText("DEL: reset", (float)rightPanelX, (float)(ctrlY + 36), 14, Color{180, 180, 180, 255});
        DrawAppText("TAB: cycle presets", (float)rightPanelX, (float)(ctrlY + 54), 14, Color{180, 180, 180, 255});

        // Draw dropdown open items on top
        if (dropdownOpen) {
            for (int p = 0; p < numPresets; p++) {
                Rectangle optRect = { dropdownRect.x, dropdownRect.y + dropdownRect.height + p * dropdownRect.height, dropdownRect.width, dropdownRect.height };
                DrawRectangleRec(optRect, hoveredDropdownIdx == p ? Color{70, 70, 100, 255} : Color{45, 45, 70, 255});
                DrawRectangleLinesEx(optRect, 1, Color{100, 100, 130, 255});
                std::string optName = positionPresets[p].name;
                DrawAppText(optName.c_str(), optRect.x + 10, optRect.y + 5, 14, hoveredDropdownIdx == p ? WHITE : Color{180, 180, 180, 255});
            }
        }

        EndDrawing();
    }

    cleanupAppFont();
    CloseWindow();
}
