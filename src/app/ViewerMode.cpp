#include "ViewerMode.h"
#include "BoardDraw.h"
#include "FontManager.h"
#include "game-suite/Board.h"
#include "trix-bots/Utils.h"
#include <string>
#include <vector>
#include <sstream>
#include <bitset>
#include <cstring>

struct GamePreset {
    const char* name;
    const char* gameCode;
};

static const GamePreset gamePresets[] = {
    {"DC vs AlphaCruncher",
     "b-10000 r-10006 b-10012 -B30500 -B30502 -W30510 -W30512 b-10606 "
     "-B30700 -B30702 -W30710 -W30712 b-11200 r-11206 b-11212 "
     "11111111111111111111 | 20 95 17 151 141 29 37 125 35 175 148 6 29 "
     "102 105 163 73 108 105 250 16 127 111 179 29 312 29 103 28 163"},
    {"Hydra with Felix Eval",
     "b-10000 r-10006 b-10012 -B30500 -B30502 -W30510 -W30512 b-10606 "
     "-B30700 -B30702 -W30710 -W30712 b-11200 r-11206 b-11212 "
     "11111111111111111111 | 41 63 93 69 104 223 100 90 126 19 92 43 120 "
     "115 175 23 102 26 169 41 177 117 144 134 115 180 97 93 66 209"},
    {"2 Hydras with Felix Eval",
     "b-10000 r-10006 b-10012 -B30500 -B30502 -W30510 -W30512 b-10606 "
     "-B30700 -B30702 -W30710 -W30712 b-11200 r-11206 b-11212 "
     "11111111111111111111 | 29 132 75 63 186 255 32 290 38 276 37 128 "
     "67 222 129 266 103 23 192 166 120 154 149 194 143 150"},
    {"Hydra vs Deepchad d5",
     "b-10000 r-10006 b-10012 -B30500 -B30502 -W30510 -W30512 b-10606 "
     "-B30700 -B30702 -W30710 -W30712 b-11200 r-11206 b-11212 "
     "11111111111111111111 | 95 61 247 156 69 53 19 62 28 21 245 51 208 "
     "52 298 29 181 109 84 340 54 44 85 223 139 71 47 2 1 28 0 263 0"},
    {"2 Hydras Fixed Eval",
     "b-10000 r-10006 b-10012 -B30500 -B30502 -W30510 -W30512 b-10606 "
     "-B30700 -B30702 -W30710 -W30712 b-11200 r-11206 b-11212 "
     "11111111111111111111 | 29 132 216 61 259 52 247 27 84 74 10 112 "
     "336 138 60 110 53 79 30 78 455 16 424 274 216 140 144 181 215 "
     "186 175 99 22 56 88 105 9 129 143 58 145 242 1 68 184 145 53 165 "
     "17 91 13 72 118 1 91 1 125 3 129 3 75 32 75 49"},
    {"Hydra Executed by DC",
     "b-10000 r-10006 b-10012 -B30500 -B30502 -W30510 -W30512 b-10606 "
     "-B30700 -B30702 -W30710 -W30712 b-11200 r-11206 b-11212 "
     "11111111111111111111 | 95 63 247 253 67 162 29 112 224 128 5 121 "
     "12 183 100 61 22 193 64 227 22 216"},
    {"DeepChad d5 Mirror",
     "b-10000 r-10006 b-10012 -B30500 -B30502 -W30510 -W30512 b-10606 "
     "-B30700 -B30702 -W30710 -W30712 b-11200 r-11206 b-11212 "
     "11111111111111111111 | 27 132 12 134 24 112 163 140 6 63 91 274 "
     "67 488 374 397 131 83 180 36 47 147 124 100 26 22 22 62 36 205 6 "
     "139 202 43 233 57 1 18 291 9 230 204 161 99 39 147 288 114 205 "
     "143 291 25 353 78 295 124 182 59 262 121 219 160 98 103 51 69 "
     "260 165 26 29 296 8 4 16 42 7 34 26 151 43 21 131 107 113 180 "
     "92 127 140 77 117 139 167 51 30 62 65 222 131 206 40 252 25 48 "
     "32 26 4 164 53 68 3 118 10 72 21 92 10 132 27 156 29 134 6 135 "
     "22 144 55 74 40 134 43 134 41 135 22 144 55 74 40 134 43 134 41 "
     "135 22 144 55 74 40 134 43 134 41 135 22 144 55 74 40 134 43 134 "
     "41 135 22 144 55 74 40 134 43 134 41 135 22 144 55 74 40 134 43 "
     "134 41 135 22 144 55 74 40 134 43 134 41 135 22 144 55 74 40 134 "
     "43 134 41 135 22 "},
    {"Man vs AI (Longmen)",
     "b-10000 r-10006 b-10012 -B30500 -B30502 -W30510 -W30512 b-10606 "
     "-B30700 -B30702 -W30710 -W30712 b-11200 r-11206 b-11212 "
     "11111111111111111111 | 27 132 12 39 134 176 24 50 26 15 45 154 51 "
     "133 114 237 542 491 151 165 19 23 25 184 341 351 451 427 168 429 "
     "449 318 464 364 308 30 216 181 177 457 415 574 158 619 66 386 413 "
     "364 3 271 24 292 87 278 186 12 258 71 96 83 38 33 93 19 191 211 "
     "54 251 209 219 261 263 94 262 79 224 218 289 206 239 184 182 27 "
     "2 135 256 86 202 53 241 12 240"},
};
static const int numGamePresets = sizeof(gamePresets) / sizeof(gamePresets[0]);

static void parseAndLoadGame(const std::string& gameCode, Board& board,
                              std::vector<std::vector<Board::xMove>>& gameReplay,
                              GamePosition& displayBoard) {
    gameReplay.clear();
    board = Board();
    std::bitset<3> set(5);
    board.rst(set);

    auto pipePos = gameCode.find("|");
    if (pipePos == std::string::npos) {
        board.loadPosition(gameCode);
        board.copyPositionTo(displayBoard);
        return;
    }

    std::string startingPos = gameCode.substr(0, pipePos - 1);
    board.loadPosition(startingPos);
    board.copyPositionTo(displayBoard);

    std::string movesStr = gameCode.substr(pipePos + 2);
    std::stringstream ss(movesStr);
    int moveIndex;
    while (ss >> moveIndex) {
        std::vector<std::vector<Board::xMove>> moves =
            board.getMoves(gameReplay.size() % 2 == 0);
        if (moveIndex < (int)moves.size()) {
            gameReplay.emplace_back(moves[moveIndex]);
            board.makeMove(gameReplay.back(), gameReplay.size() % 2 == 1);
        }
    }

    board.loadPosition(startingPos);
    board.copyPositionTo(displayBoard);
}

void ViewerMode::run(const AppConfig& config) {
    const int boardPx = 13 * cellSize;
    const int rightPanelX = boardPx + 20;
    const int rightPanelW = 300;
    const int windowWidth = rightPanelX + rightPanelW + 20;
    const int windowHeight = boardPx;
    const Rectangle dropdownRect = { (float)rightPanelX, 20.0f, (float)rightPanelW, 26.0f };

    InitWindow(windowWidth, windowHeight, "Crosstones V4 - Game Viewer");
    SetTargetFPS(30);
    initAppFont();

    // Find starting preset
    int presetIdx = 0;
    for (int p = 0; p < numGamePresets; p++) {
        if (config.viewer.gameCode == gamePresets[p].gameCode) {
            presetIdx = p;
            break;
        }
    }

    GamePosition displayBoard[3] = {0};
    Board board;
    std::vector<std::vector<Board::xMove>> gameReplay;
    int currentMove = 0;
    bool dropdownOpen = false;
    int hoveredDropdownIdx = -1;

    parseAndLoadGame(gamePresets[presetIdx].gameCode, board, gameReplay,
                     displayBoard[0]);

    while (!WindowShouldClose()) {
        Vector2 mouse = GetMousePosition();
        bool click = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);

        // --- Handle input ---
        if (!dropdownOpen) {
            if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) {
                if (currentMove < (int)gameReplay.size()) {
                    board.unsafeMakeMove(gameReplay[currentMove]);
                    board.copyPositionTo(displayBoard[0]);
                    currentMove++;
                }
            }
            if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) {
                if (currentMove > 0) {
                    board.unsafeMakeMove(gameReplay[currentMove - 1]);
                    board.copyPositionTo(displayBoard[0]);
                    currentMove--;
                }
            }
            if (IsKeyPressed(KEY_BACKSPACE)) {
                while (currentMove > 0) {
                    board.unsafeMakeMove(gameReplay[currentMove - 1]);
                    currentMove--;
                }
                board.copyPositionTo(displayBoard[0]);
            }
        }

        // Preset cycling
        bool presetChanged = false;
        if (IsKeyPressed(KEY_LEFT_BRACKET)) {
            presetIdx = (presetIdx - 1 + numGamePresets) % numGamePresets;
            presetChanged = true;
        } else if (IsKeyPressed(KEY_RIGHT_BRACKET) || IsKeyPressed(KEY_TAB)) {
            presetIdx = (presetIdx + 1) % numGamePresets;
            presetChanged = true;
        }

        // Dropdown interaction
        hoveredDropdownIdx = -1;
        if (dropdownOpen) {
            for (int p = 0; p < numGamePresets; p++) {
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
            parseAndLoadGame(gamePresets[presetIdx].gameCode, board, gameReplay,
                             displayBoard[0]);
            currentMove = 0;
        }

        // --- Build highlights ---
        bool highlights[13][13] = {false};
        if (currentMove > 0) {
            for (const auto& xm : gameReplay[currentMove - 1]) {
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
        if (gameReplay.size() > 0) {
            std::string moveInfo = "Move: " + std::to_string(currentMove) +
                                   " / " + std::to_string(gameReplay.size());
            DrawAppText(moveInfo.c_str(), (float)rightPanelX, 56.0f, 16, WHITE);
        } else {
            DrawAppText("No moves in this game", (float)rightPanelX, 56.0f, 16, YELLOW);
        }

        // Dropdown base button
        DrawRectangleRec(dropdownRect, dropdownOpen ? Color{70, 70, 100, 255} : Color{50, 50, 75, 255});
        DrawRectangleLinesEx(dropdownRect, 1, Color{100, 100, 130, 255});
        std::string presetName = gamePresets[presetIdx].name;
        float nameW = MeasureAppText(presetName.c_str(), 16);
        DrawAppText(presetName.c_str(), dropdownRect.x + dropdownRect.width / 2.0f - nameW / 2.0f, dropdownRect.y + 5, 16, WHITE);
        DrawAppText(dropdownOpen ? "^" : "v", dropdownRect.x + dropdownRect.width - 20, dropdownRect.y + 5, 16, WHITE);

        // Controls
        int ctrlY = 86;
        DrawAppText("A/D: step moves", (float)rightPanelX, (float)ctrlY, 14, Color{180, 180, 180, 255});
        DrawAppText("BACKSPACE: rewind", (float)rightPanelX, (float)(ctrlY + 18), 14, Color{180, 180, 180, 255});
        DrawAppText("TAB: cycle games", (float)rightPanelX, (float)(ctrlY + 36), 14, Color{180, 180, 180, 255});

        // Draw dropdown open items on top
        if (dropdownOpen) {
            for (int p = 0; p < numGamePresets; p++) {
                Rectangle optRect = { dropdownRect.x, dropdownRect.y + dropdownRect.height + p * dropdownRect.height, dropdownRect.width, dropdownRect.height };
                DrawRectangleRec(optRect, hoveredDropdownIdx == p ? Color{70, 70, 100, 255} : Color{45, 45, 70, 255});
                DrawRectangleLinesEx(optRect, 1, Color{100, 100, 130, 255});
                std::string optName = gamePresets[p].name;
                DrawAppText(optName.c_str(), optRect.x + 10, optRect.y + 5, 14, hoveredDropdownIdx == p ? WHITE : Color{180, 180, 180, 255});
            }
        }

        EndDrawing();
    }

    cleanupAppFont();
    CloseWindow();
}
