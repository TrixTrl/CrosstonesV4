#include "MoveExplorerApp.h"
#include "raylib.h"
#include "ui/components/panel.h"
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
    layout = { .dir = ui::Dir::Row, .children = {
        { .id = "board", .flex = 1, .minPx = 600 },
        { .id = "panel", .fixed = 300 },
    }};

    state = State{};
    if (!loadedPosition.position.empty()) {
        for (int p = 0; p < numPresets; p++) {
            if (loadedPosition.position == positionPresets[p].position) {
                state.presetIdx = p;
                break;
            }
        }
    }

    std::bitset<3> set(5);
    board.rst(set);
    board.loadPosition(positionPresets[state.presetIdx].position);
    board.copyPositionTo(boardView.position());
}

void MoveExplorerApp::onTick(float dt, const InputState& input) {
    bool isWhite = false;
    auto moves = board.getMoves(isWhite);

    bool moveRequested = false;
    int moveDelta = 0;

    if (!dropdown.open) {
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
            state.moveIdx = 0;
            moveRequested = true;
        } else if (IsKeyPressed(KEY_SPACE)) {
            state.paused = !state.paused;
            state.autoTimer = 0;
        }
    }

    bool presetChanged = false;
    if (IsKeyPressed(KEY_LEFT_BRACKET)) {
        state.presetIdx = (state.presetIdx - 1 + numPresets) % numPresets;
        presetChanged = true;
    } else if (IsKeyPressed(KEY_RIGHT_BRACKET) || IsKeyPressed(KEY_TAB)) {
        state.presetIdx = (state.presetIdx + 1) % numPresets;
        presetChanged = true;
    }

    Rectangle panelRect = layout.find("panel")->rect;
    Rectangle dropdownRect = { panelRect.x, 20.0f * theme.scale, panelRect.width, (float)theme.itemH };
    int sel = dropdown.handleInput(dropdownRect, input.mouse, input.clicked, numPresets);
    if (sel >= 0) {
        state.presetIdx = sel;
        presetChanged = true;
    }

    if (presetChanged) {
        board = Board();
        std::bitset<3> set(5);
        board.rst(set);
        board.loadPosition(positionPresets[state.presetIdx].position);
        board.copyPositionTo(boardView.position());
        moves = board.getMoves(isWhite);
        state.moveIdx = 0;
        state.paused = false;
        state.autoTimer = 0;
    }

    if (!state.paused && !dropdown.open && moves.size() > 0) {
        state.autoTimer++;
        if (state.autoTimer >= 30) {
            state.autoTimer = 0;
            moveDelta = 1;
            moveRequested = true;
        }
    }

    if (moves.size() > 0) {
        if (moveRequested) {
            state.moveIdx += moveDelta;
            state.moveIdx = (state.moveIdx % (int)moves.size() + (int)moves.size()) % (int)moves.size();
        }
        board.copyPositionTo(boardView.position());
        Board tempBoard = board;
        tempBoard.unsafeApplyMove(boardView.position(), moves[state.moveIdx]);
    } else {
        board.copyPositionTo(boardView.position());
    }

    bool hl[13][13]{};
    if (moves.size() > 0) {
        for (const auto& xm : moves[state.moveIdx]) {
            if (xm.i >= 0 && xm.i < 13 && xm.j >= 0 && xm.j < 13)
                hl[xm.i][xm.j] = true;
        }
    }
    boardView.setHighlights(hl);
}

void MoveExplorerApp::onDraw(Rectangle rect) {
    ui::Slot* boardSlot = layout.find("board");
    boardView.draw(boardSlot ? boardSlot->rect : rect, theme.scale);
}

void MoveExplorerApp::onDrawOverlay(Rectangle rect) {
    auto& r = layout.find("panel")->rect;
    ui::Panel::draw(r, "Position", theme);

    std::vector<std::string_view> presetNames;
    for (int p = 0; p < numPresets; p++)
        presetNames.push_back(positionPresets[p].name);
    Rectangle dropdownRect = { r.x, r.y + 24 * theme.scale, r.width, (float)theme.itemH };
    dropdown.draw(dropdownRect, presetNames, theme);

    auto moves = board.getMoves(false);
    std::string moveInfo = "Move: " + std::to_string(state.moveIdx + 1) + " / " + std::to_string(moves.size());
    if (state.paused) moveInfo += "  [PAUSED]";
    drawText(moveInfo.c_str(), (int)r.x, (int)(r.y + 56 * theme.scale), theme.fontSizeTitle, theme.text);

    int ctrlY = (int)(r.y + 86 * theme.scale);
    drawText("SPACE: pause/resume", (int)r.x, ctrlY, theme.fontSize, theme.textDim);
    drawText("A/D: navigate", (int)r.x, (int)(ctrlY + 18 * theme.scale), theme.fontSize, theme.textDim);
    drawText("DEL: reset", (int)r.x, (int)(ctrlY + 36 * theme.scale), theme.fontSize, theme.textDim);
    drawText("TAB: cycle presets", (int)r.x, (int)(ctrlY + 54 * theme.scale), theme.fontSize, theme.textDim);
}
