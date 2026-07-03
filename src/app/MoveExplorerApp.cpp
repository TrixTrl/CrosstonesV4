#include "MoveExplorerApp.h"
#include "raylib.h"
#include "game-suite/Board.h"
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

void MoveExplorerApp::onStart() {
    presetIdx = 0;
    for (int p = 0; p < numPresets; p++) {
        if (config->explore.position == positionPresets[p].position) {
            presetIdx = p;
            break;
        }
    }

    std::bitset<3> set(5);
    board.rst(set);
    board.loadPosition(positionPresets[presetIdx].position);
    board.copyPositionTo(displayBoard);
}

void MoveExplorerApp::onFrame() {
    bool isWhite = false;
    auto moves = board.getMoves(isWhite);

    Vector2 mouse = GetMousePosition();
    bool click = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);

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
            moveIdx = 0;
            moveRequested = true;
        } else if (IsKeyPressed(KEY_SPACE)) {
            paused = !paused;
            autoTimer = 0;
        }
    }

    bool presetChanged = false;
    if (IsKeyPressed(KEY_LEFT_BRACKET)) {
        presetIdx = (presetIdx - 1 + numPresets) % numPresets;
        presetChanged = true;
    } else if (IsKeyPressed(KEY_RIGHT_BRACKET) || IsKeyPressed(KEY_TAB)) {
        presetIdx = (presetIdx + 1) % numPresets;
        presetChanged = true;
    }

    Rectangle dropdownRect = { (float)panelX(), 20.0f, (float)panelW(), 26.0f };
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
        board = Board();
        std::bitset<3> set(5);
        board.rst(set);
        board.loadPosition(positionPresets[presetIdx].position);
        board.copyPositionTo(displayBoard);
        moves = board.getMoves(isWhite);
        moveIdx = 0;
        paused = false;
        autoTimer = 0;
    }

    if (!paused && !dropdownOpen && moves.size() > 0) {
        autoTimer++;
        if (autoTimer >= 30) {
            autoTimer = 0;
            moveDelta = 1;
            moveRequested = true;
        }
    }

    if (moves.size() > 0) {
        if (moveRequested) {
            moveIdx += moveDelta;
            moveIdx = (moveIdx % (int)moves.size() + (int)moves.size()) % (int)moves.size();
        }
        board.copyPositionTo(displayBoard);
        Board tempBoard = board;
        tempBoard.unsafeApplyMove(displayBoard, moves[moveIdx]);
    } else {
        board.copyPositionTo(displayBoard);
    }

    std::memset(highlights, 0, sizeof(highlights));
    if (moves.size() > 0) {
        for (const auto& xm : moves[moveIdx]) {
            if (xm.i >= 0 && xm.i < 13 && xm.j >= 0 && xm.j < 13)
                highlights[xm.i][xm.j] = true;
        }
    }

    App::onFrame();
}

void MoveExplorerApp::onDrawOverlay() {
    drawRightPanel();

    Rectangle dropdownRect = { (float)panelX(), 20.0f, (float)panelW(), 26.0f };

    auto moves = board.getMoves(false);
    std::string moveInfo = "Move: " + std::to_string(moveIdx + 1) + " / " + std::to_string(moves.size());
    if (paused) moveInfo += "  [PAUSED]";
    drawText(moveInfo.c_str(), panelX(), 56, 16, WHITE);

    DrawRectangleRec(dropdownRect, dropdownOpen ? Color{70, 70, 100, 255} : Color{50, 50, 75, 255});
    DrawRectangleLinesEx(dropdownRect, 1, Color{100, 100, 130, 255});
    std::string presetName = positionPresets[presetIdx].name;
    float nameW = measureText(presetName.c_str(), 16);
    DrawAppText(presetName.c_str(), dropdownRect.x + dropdownRect.width / 2.0f - nameW / 2.0f, dropdownRect.y + 5, 16, WHITE);
    DrawAppText(dropdownOpen ? "^" : "v", dropdownRect.x + dropdownRect.width - 20, dropdownRect.y + 5, 16, WHITE);

    int ctrlY = 86;
    drawText("SPACE: pause/resume", panelX(), ctrlY, 14, Color{180, 180, 180, 255});
    drawText("A/D: navigate", panelX(), ctrlY + 18, 14, Color{180, 180, 180, 255});
    drawText("DEL: reset", panelX(), ctrlY + 36, 14, Color{180, 180, 180, 255});
    drawText("TAB: cycle presets", panelX(), ctrlY + 54, 14, Color{180, 180, 180, 255});

    if (dropdownOpen) {
        for (int p = 0; p < numPresets; p++) {
            Rectangle optRect = { dropdownRect.x, dropdownRect.y + dropdownRect.height + p * dropdownRect.height, dropdownRect.width, dropdownRect.height };
            DrawRectangleRec(optRect, hoveredDropdownIdx == p ? Color{70, 70, 100, 255} : Color{45, 45, 70, 255});
            DrawRectangleLinesEx(optRect, 1, Color{100, 100, 130, 255});
            std::string optName = positionPresets[p].name;
            DrawAppText(optName.c_str(), optRect.x + 10, optRect.y + 5, 14, hoveredDropdownIdx == p ? WHITE : Color{180, 180, 180, 255});
        }
    }
}
