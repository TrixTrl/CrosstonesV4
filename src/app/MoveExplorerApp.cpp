#include "MoveExplorerApp.h"
#include "raylib.h"
#include "ui/components/panel.h"
#include "game-suite/Board.h"
#include <string>
#include <vector>
#include <cstring>

void MoveExplorerApp::onStart() {
    layout = { .dir = ui::Dir::Row, .children = {
        { .id = "board", .flex = 1, .minPx = 600 },
        { .id = "panel", .fixed = 300 },
    }};

    presets = PositionPresets::loadAll();

    state = State{};
    if (!loadedPosition.position.empty()) {
        for (int p = 0; p < (int)presets.size(); p++) {
            if (loadedPosition.position == presets[p].position) {
                state.presetIdx = p;
                break;
            }
        }
    }

    board.rst(GameMode::Orchid);
    if (!presets.empty())
        board.loadPosition(presets[state.presetIdx].position);
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

    int numPresets = (int)presets.size();
    bool presetChanged = false;
    if (numPresets > 0 && IsKeyPressed(KEY_LEFT_BRACKET)) {
        state.presetIdx = (state.presetIdx - 1 + numPresets) % numPresets;
        presetChanged = true;
    } else if (numPresets > 0 && (IsKeyPressed(KEY_RIGHT_BRACKET) || IsKeyPressed(KEY_TAB))) {
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

    if (presetChanged && numPresets > 0) {
        board = Board();
        board.rst(GameMode::Orchid);
        board.loadPosition(presets[state.presetIdx].position);
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
    boardView.draw(boardSlot ? boardSlot->rect : rect, theme.scale, theme.highlight);
}

void MoveExplorerApp::onDrawOverlay(Rectangle rect) {
    auto& r = layout.find("panel")->rect;
    ui::Panel::draw(r, "Position", theme);

    std::vector<std::string_view> presetNames;
    for (auto& p : presets)
        presetNames.push_back(p.name);
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
