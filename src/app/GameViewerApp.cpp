#include "GameViewerApp.h"
#include "raylib.h"
#include "ui/components/panel.h"
#include <string>
#include <sstream>
#include <bitset>
#include <cstring>

void GameViewerApp::parseAndLoadGame(const std::string& gameCode) {
    gameReplay.clear();
    board = Board();
    std::bitset<3> set(5);
    board.rst(set);

    auto pipePos = gameCode.find("|");
    if (pipePos == std::string::npos) {
        board.loadPosition(gameCode);
        board.copyPositionTo(boardView.position());
        return;
    }

    std::string startingPos = gameCode.substr(0, pipePos - 1);
    board.loadPosition(startingPos);
    board.copyPositionTo(boardView.position());

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
    board.copyPositionTo(boardView.position());
}

static std::string buildGameCode(const std::string& rootPos,
                                  const std::vector<int>& moves) {
    std::string code = rootPos + " |";
    for (int m : moves)
        code += " " + std::to_string(m);
    return code;
}

void GameViewerApp::onStart() {
    layout = { .dir = ui::Dir::Row, .children = {
        { .id = "board", .flex = 1, .minPx = 600 },
        { .id = "panel", .fixed = 300 },
    }};

    auto all = GameEntries::loadAll();

    entries.clear();
    for (auto& s : all.saves) {
        std::string pos = s.position.empty() ? all.rootPos : s.position;
        entries.push_back({s.name, buildGameCode(pos, s.moves)});
    }

    state = State{};
    if (!loadedPosition.displayName.empty()) {
        for (size_t i = 0; i < entries.size(); i++) {
            if (entries[i].name == loadedPosition.displayName) {
                state.presetIdx = (int)i;
                break;
            }
        }
    }

    if (!entries.empty())
        parseAndLoadGame(entries[state.presetIdx].gameCode);
    state.currentMove = 0;
}

void GameViewerApp::onTick(float dt, const InputState& input) {
    if (entries.empty()) return;

    if (!dropdown.open) {
        if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) {
            if (state.currentMove < (int)gameReplay.size()) {
                board.unsafeMakeMove(gameReplay[state.currentMove]);
                board.copyPositionTo(boardView.position());
                state.currentMove++;
            }
        }
        if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) {
            if (state.currentMove > 0) {
                board.unsafeMakeMove(gameReplay[state.currentMove - 1]);
                board.copyPositionTo(boardView.position());
                state.currentMove--;
            }
        }
        if (IsKeyPressed(KEY_BACKSPACE)) {
            while (state.currentMove > 0) {
                board.unsafeMakeMove(gameReplay[state.currentMove - 1]);
                state.currentMove--;
            }
            board.copyPositionTo(boardView.position());
        }
    }

    bool presetChanged = false;
    int numEntries = (int)entries.size();
    if (IsKeyPressed(KEY_LEFT_BRACKET)) {
        state.presetIdx = (state.presetIdx - 1 + numEntries) % numEntries;
        presetChanged = true;
    } else if (IsKeyPressed(KEY_RIGHT_BRACKET) || IsKeyPressed(KEY_TAB)) {
        state.presetIdx = (state.presetIdx + 1) % numEntries;
        presetChanged = true;
    }

    Rectangle panelRect = layout.find("panel")->rect;
    Rectangle dropdownRect = { panelRect.x, 20.0f * theme.scale, panelRect.width, (float)theme.itemH };
    int sel = dropdown.handleInput(dropdownRect, input.mouse, input.clicked, numEntries);
    if (sel >= 0) {
        state.presetIdx = sel;
        presetChanged = true;
    }

    if (presetChanged) {
        parseAndLoadGame(entries[state.presetIdx].gameCode);
        state.currentMove = 0;
    }

    // Build highlights
    bool hl[13][13]{};
    if (state.currentMove > 0) {
        for (const auto& xm : gameReplay[state.currentMove - 1]) {
            if (xm.i >= 0 && xm.i < 13 && xm.j >= 0 && xm.j < 13)
                hl[xm.i][xm.j] = true;
        }
    }
    boardView.setHighlights(hl);
}

void GameViewerApp::onDraw(Rectangle rect) {
    ui::Slot* boardSlot = layout.find("board");
    boardView.draw(boardSlot ? boardSlot->rect : rect, theme.scale);
}

void GameViewerApp::onDrawOverlay(Rectangle rect) {
    auto& r = layout.find("panel")->rect;
    ui::Panel::draw(r, "Game", theme);

    std::vector<std::string_view> entryNames;
    for (auto& e : entries)
        entryNames.push_back(e.name);
    Rectangle dropdownRect = { r.x, r.y + 24 * theme.scale, r.width, (float)theme.itemH };
    dropdown.draw(dropdownRect, entryNames, theme);

    if (gameReplay.size() > 0) {
        std::string moveInfo = "Move: " + std::to_string(state.currentMove) +
                               " / " + std::to_string(gameReplay.size());
        drawText(moveInfo.c_str(), (int)r.x, (int)(r.y + 56 * theme.scale), (int)theme.fontSizeTitle, theme.text);
    } else {
        drawText("No moves in this game", (int)r.x, (int)(r.y + 56 * theme.scale), (int)theme.fontSizeTitle, YELLOW);
    }

    int ctrlY = (int)(r.y + 86 * theme.scale);
    drawText("A/D: step moves", (int)r.x, ctrlY, theme.fontSize, theme.textDim);
    drawText("BACKSPACE: rewind", (int)r.x, (int)(ctrlY + 18 * theme.scale), theme.fontSize, theme.textDim);
    drawText("TAB: cycle games", (int)r.x, (int)(ctrlY + 36 * theme.scale), theme.fontSize, theme.textDim);
}
