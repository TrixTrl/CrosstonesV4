#include "opening_explorer.h"
#include "raylib.h"
#include "app/FontManager.h"
#include "app/ui/components/panel.h"
#include <fstream>
#include <sstream>
#include <bitset>
#include <cstring>
#include <algorithm>

// ── helpers ──────────────────────────────────────────────────────────

static const char* defaultRootPos() {
    return "b-10000 r-10006 b-10012 -B30500 -B30502 -W30510 -W30512 "
           "b-10606 -B30700 -B30702 -W30710 -W30712 b-11200 r-11206 b-11212 "
           "11111111111111111111";
}

static const char* gameModeNameForBits(int bits) {
    switch (bits) {
        case 0: return "Phoenix";
        case 1: return "Monkey";
        case 2: return "Lotus";
        case 3: return "Kylin";
        case 4: return "Tortoise";
        case 5: return "Orchid";
        case 6: return "Crane";
        case 7: return "Dragon";
        default: return "Custom";
    }
}

static int extractGameModeBits(const std::string& pos) {
    auto space = pos.rfind(' ');
    if (space == std::string::npos) return 2;
    std::string last = pos.substr(space + 1);
    if (last.length() >= 3) {
        int b = 0;
        for (int i = 0; i < 3 && i < (int)last.length(); i++)
            if (last[i] == '1') b |= (1 << (2 - i));
        return b;
    }
    return 2;
}

// ── loading (old-format compatible) ──────────────────────────────────

static void parseOpeningsFile(const std::string& path,
                              std::string& outRootPos,
                              std::vector<std::pair<std::string, std::vector<int>>>& outOpenings) {
    std::ifstream file(path);
    if (!file.is_open()) return;

    std::string line, currentName;
    std::vector<int> currentMoves;

    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        if (line.substr(0, 5) == "root=") {
            outRootPos = line.substr(5);
        } else if (line[0] == '[') {
            if (!currentName.empty() && !currentMoves.empty())
                outOpenings.push_back({currentName, currentMoves});
            auto end = line.find(']');
            currentName = line.substr(1, end == std::string::npos ? line.size() - 1 : end - 1);
            currentMoves.clear();
        } else if (line.substr(0, 6) == "moves=") {
            std::string ms = line.substr(6);
            std::stringstream ss(ms);
            int idx;
            while (ss >> idx) currentMoves.push_back(idx);
        }
    }
    if (!currentName.empty() && !currentMoves.empty())
        outOpenings.push_back({currentName, currentMoves});
}

// ── onStart ──────────────────────────────────────────────────────────

void OpeningExplorer::onStart() {
    layout = {
        .dir = ui::Dir::Col,
        .children = {
            { .id = "topbar",   .fixed = 40 },
            { .id = "content",  .flex = 1, .dir = ui::Dir::Row, .children = {
                { .id = "left", .flex = 3, .dir = ui::Dir::Col, .children = {
                    { .id = "board",    .flex = 1 },
                    { .id = "analysis", .fixed = 160 },
                }},
                { .id = "right", .flex = 1, .dir = ui::Dir::Col, .children = {
                    { .id = "history",  .flex = 3, .minPx = 80 },
                    { .id = "moves",    .flex = 4, .minPx = 80 },
                }},
            }},
            { .id = "bottombar", .fixed = 30 },
        }
    };

    loadAndBuildTree();
    phase = ExplorerPhase::ChoosingMode;
    selectedGameMode = -1;
    onBoardStateChanged();
}

// ── loadAndBuildTree ─────────────────────────────────────────────────

void OpeningExplorer::initDefaultRoot() {
    rootPos = defaultRootPos();
    gameModeName = gameModeNameForBits(extractGameModeBits(rootPos));
}

void OpeningExplorer::loadAndBuildTree() {
    rootPos.clear();
    std::vector<std::pair<std::string, std::vector<int>>> openings;

    parseOpeningsFile("resources/openings.txt", rootPos, openings);

    if (rootPos.empty()) {
        initDefaultRoot();
    } else {
        gameModeName = gameModeNameForBits(extractGameModeBits(rootPos));
    }

    buildTreeFromOpenings(openings);
    currentNode = &root;
}

void OpeningExplorer::buildTreeFromOpenings(
    const std::vector<std::pair<std::string, std::vector<int>>>& openings)
{
    root = MoveNode{};
    root.name = "Root";

    for (const auto& [name, indices] : openings) {
        Board b;
        std::bitset<3> set(5);
        b.rst(set);
        b.loadPosition(rootPos);

        MoveNode* node = &root;
        for (size_t mi = 0; mi < indices.size(); ++mi) {
            bool isWhite = (mi % 2 == 0);
            auto allMoves = b.getMoves(isWhite);
            int idx = indices[mi];

            if (idx < 0 || idx >= (int)allMoves.size()) break;

            if (allMoves[idx].empty()) break;

            auto* child = node->findChild(allMoves[idx]);
            if (!child) child = node->addChild(allMoves[idx]);

            b.unsafeMakeMove(allMoves[idx]);
            node = child;
        }
        node->name = name;
    }
}

static std::string makeModeToken(int gameMode) {
    int groups[] = {
        0, 0, 0, 0, 2, 0, 0, 2, 1, 1,
        1, 1, 2, 0, 0, 2, 0, 0, 0, 0,
    };
    bool bits[3] = {
        ((gameMode >> 0) & 1) != 0,
        ((gameMode >> 1) & 1) != 0,
        ((gameMode >> 2) & 1) != 0,
    };
    std::string token(20, '0');
    for (int n = 0; n < 20; n++)
        token[n] = bits[groups[n]] ? '1' : '0';
    return token;
}

void OpeningExplorer::initBoardAtRoot(int gameMode) {
    board = Board();
    std::bitset<3> set(gameMode);
    board.rst(set);
    std::string pos = rootPos;
    size_t space = pos.rfind(' ');
    if (space != std::string::npos)
        pos = pos.substr(0, space + 1) + makeModeToken(gameMode);
    board.loadPosition(pos);
    gameModeName = gameModeNameForBits(gameMode);
}

void OpeningExplorer::syncBoard() {
    board.copyPositionTo(boardView.position());
}

// ── build UI data ───────────────────────────────────────────────────

static const char* modeNames[] = {
    "Phoenix", "Monkey", "Lotus", "Kylin",
    "Tortoise", "Orchid", "Crane", "Dragon"
};

void OpeningExplorer::buildMoveList() {
    moveList.entries.clear();

    if (phase == ExplorerPhase::ChoosingMode) {
        for (int i = 0; i < 8; i++) {
            ui::MoveEntry e;
            e.label = modeNames[i];
            e.eval = 0.0f;
            e.isBook = true;
            moveList.entries.push_back(e);
        }
        return;
    }

    bool isWhite = (navPath.size() % 2 == 0);
    cachedKsnMoves = KsnFormatter::enrichAll(RichMoveGenerator::generate(board.positionCopy(), isWhite));

    for (int i = 0; i < (int)cachedKsnMoves.size(); i++) {
        ui::MoveEntry e;
        e.label = KsnFormatter::shorten(i, cachedKsnMoves);
        e.eval = defaultEval.evaluate(board, cachedKsnMoves[i].info.xmove);
        e.originalIndex = i;
        e.userData = &cachedKsnMoves[i].info.xmove;

        if (currentNode && !cachedKsnMoves[i].info.xmove.empty()) {
            for (const auto& child : currentNode->children) {
                if (child->moveData == cachedKsnMoves[i].info.xmove) {
                    e.isBook = true;
                    break;
                }
            }
        }
        moveList.entries.push_back(e);
    }
    if (currentNode) {
        for (auto& e : moveList.entries) {
            if (!e.userData) continue;
            auto* md = static_cast<XMove*>(e.userData);
            if (md->empty()) continue;
            for (const auto& child : currentNode->children) {
                if (child->moveData == (*md) && !child->name.empty()) {
                    e.subtitle = child->name;
                    break;
                }
            }
        }
    }

    std::stable_sort(moveList.entries.begin(), moveList.entries.end(),
        [](const ui::MoveEntry& a, const ui::MoveEntry& b) {
            if (a.isBook != b.isBook) return a.isBook > b.isBook;
            return a.eval > b.eval;
        });
}

void OpeningExplorer::buildMoveHistory() {
    moveHistory.rows.clear();

    ui::HistoryRow r0;
    r0.moveNumber = 0;
    if (phase == ExplorerPhase::ChoosingMode) {
        r0.white = "<choose mode>";
        r0.black = "";
        moveHistory.rows.push_back(r0);
        return;
    }
    r0.white = "...";
    r0.black = gameModeName;
    moveHistory.rows.push_back(r0);

    int pairs = ((int)navPath.size() + 1) / 2;
    for (int i = 0; i < pairs; i++) {
        ui::HistoryRow row;
        row.moveNumber = i + 1;

        int wIdx = i * 2;
        int bIdx = i * 2 + 1;

        if (wIdx < (int)navPath.size())
            row.white = navPath[wIdx].ksn;
        if (bIdx < (int)navPath.size())
            row.black = navPath[bIdx].ksn;

        moveHistory.rows.push_back(row);
    }
}

// ── navigation ──────────────────────────────────────────────────────

void OpeningExplorer::onBoardStateChanged() {
    buildMoveList();
    buildMoveHistory();
}

void OpeningExplorer::playMove(int candidateIdx) {
    if (candidateIdx < 0 || candidateIdx >= (int)moveList.entries.size()) return;

    if (phase == ExplorerPhase::ChoosingMode) {
        if (candidateIdx >= 8) return;
        selectedGameMode = candidateIdx;
        lastSelectedMode = selectedGameMode;
        phase = ExplorerPhase::Exploring;
        initBoardAtRoot(selectedGameMode);
        currentNode = &root;
        syncBoard();
        onBoardStateChanged();
        return;
    }

    int originalIdx = moveList.entries[candidateIdx].originalIndex;

    if (originalIdx < 0 || originalIdx >= (int)cachedKsnMoves.size()) return;
    const auto& ksnMove = cachedKsnMoves[originalIdx];

    MoveNode* nextNode = nullptr;
    if (!ksnMove.info.xmove.empty() && currentNode) {
        nextNode = currentNode->findChild(ksnMove.info.xmove);
    }

    if (!ksnMove.info.xmove.empty()) {
        NavStep step;
        step.move = ksnMove.info.xmove;
        step.info = ksnMove.info;
        step.node = nextNode;
        step.ksn = ksnMove.longKsn;
        navPath.push_back(step);
    }

    board.unsafeMakeMove(ksnMove.info.xmove);
    currentNode = nextNode;
    syncBoard();

    onBoardStateChanged();
}

void OpeningExplorer::navigateBackTo(int historyRow) {
    if (historyRow == 0 && phase == ExplorerPhase::Exploring) {
        phase = ExplorerPhase::ChoosingMode;
        selectedGameMode = -1;
        navPath.clear();
        currentNode = &root;
        initBoardAtRoot(lastSelectedMode);
        onBoardStateChanged();
        return;
    }
    if (historyRow < 0) return;

    int targetMoves = (historyRow == 0) ? 0
                     : (std::min)(historyRow * 2, (int)navPath.size());

    undoToMoveCount(targetMoves);
}

void OpeningExplorer::undoToMoveCount(int targetMoves) {
    if (targetMoves < 0 || targetMoves >= (int)navPath.size()) return;

    navPath.resize(targetMoves);

    Board b;
    int gm = (phase == ExplorerPhase::Exploring) ? selectedGameMode : 2;
    std::bitset<3> set(gm);
    b.rst(set);
    std::string pos = rootPos;
    size_t spc = pos.rfind(' ');
    if (spc != std::string::npos)
        pos = pos.substr(0, spc + 1) + makeModeToken(gm);
    b.loadPosition(pos);

    MoveNode* node = &root;
    for (auto& step : navPath) {
        b.unsafeMakeMove(step.move);
        if (step.node) node = step.node;
        else node = nullptr;
    }

    currentNode = node;
    board = std::move(b);
    syncBoard();

    onBoardStateChanged();
}

std::string OpeningExplorer::getOpeningName() const {
    if (phase == ExplorerPhase::ChoosingMode) return "Select game mode";
    std::string name;
    for (auto& step : navPath)
        if (step.node && !step.node->name.empty())
            name = step.node->name;
    return name.empty() ? "(unknown)" : name;
}

// ── onTick ──────────────────────────────────────────────────────────

void OpeningExplorer::onTick(float dt, const InputState& input) {
    Vector2 mouse = input.mouse;
    bool click = input.clicked;

    HitRects hr = computeHitRects();

    // ── keyboard navigation ──
    {
        bool upHeld = IsKeyDown(KEY_UP) || IsKeyDown(KEY_W);
        bool downHeld = IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S);
        bool upPressed = IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W);
        bool downPressed = IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S);

        if (upPressed) {
            moveList.hoveredIdx = (std::max)(0, moveList.hoveredIdx - 1);
            keyboardHoverActive = true;
            navCooldown = 0.3f;
        } else if (downPressed) {
            moveList.hoveredIdx = (std::min)((int)moveList.entries.size() - 1,
                                           moveList.hoveredIdx + 1);
            keyboardHoverActive = true;
            navCooldown = 0.3f;
        } else if (upHeld || downHeld) {
            keyboardHoverActive = true;
            navCooldown -= dt;
            while (navCooldown <= 0) {
                if (upHeld)
                    moveList.hoveredIdx = (std::max)(0, moveList.hoveredIdx - 1);
                else
                    moveList.hoveredIdx = (std::min)((int)moveList.entries.size() - 1,
                                                   moveList.hoveredIdx + 1);
                navCooldown += 0.07f;
            }
        } else {
            navCooldown = 0;
        }

        if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D) || IsKeyPressed(KEY_ENTER)) {
            if (moveList.hoveredIdx >= 0 && moveList.hoveredIdx < (int)moveList.entries.size())
                playMove(moveList.hoveredIdx);
        } else if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) {
            if (phase == ExplorerPhase::Exploring && navPath.empty())
                navigateBackTo(0);
            else if (!navPath.empty())
                undoToMoveCount((int)navPath.size() - 1);
        } else if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE) || IsKeyPressed(KEY_DELETE)) {
            phase = ExplorerPhase::ChoosingMode;
            selectedGameMode = -1;
            navPath.clear();
            currentNode = &root;
            initBoardAtRoot(lastSelectedMode);
            onBoardStateChanged();
        }
    }

    // ── mouse: move list clicks ──
    {
        int n = (int)moveList.entries.size();
        bool mouseInList = mouse.x >= hr.listX && mouse.x < hr.listX + hr.listW &&
                           mouse.y >= hr.listY && mouse.y < hr.listAreaBot;

        if (mouseInList) {
            moveList.hoveredIdx = -1;
            for (int i = 0; i < n; i++) {
                int ry = hr.listY + i * theme.itemH + (int)scrollArea.scroll.y;
                if (ry + theme.itemH < hr.listY || ry > hr.listAreaBot) continue;
                Rectangle r = {(float)hr.listX, (float)ry, (float)hr.listW, (float)theme.itemH};
                if (CheckCollisionPointRec(mouse, r)) {
                    moveList.hoveredIdx = i;
                    keyboardHoverActive = false;
                    if (click) { playMove(i); break; }
                }
            }
        } else if (!keyboardHoverActive) {
            moveList.hoveredIdx = -1;
        }
    }

    // ── mouse: history clicks ──
    {
        if (click && mouse.x >= hr.right.x && mouse.x < hr.right.x + hr.right.width)
            keyboardHoverActive = false;

        int n = (int)moveHistory.rows.size();

        moveHistory.hoveredIdx = -1;
        moveHistory.hoveredCell = -1;
        for (int i = 0; i < n; i++) {
            int ry = hr.histY + i * theme.itemH;

            Rectangle whiteRect = {(float)hr.histX, (float)ry, (float)(hr.colSplit - hr.histX), (float)theme.itemH};
            if (CheckCollisionPointRec(mouse, whiteRect)) {
                moveHistory.hoveredIdx = i;
                moveHistory.hoveredCell = 0;
                if (click) {
                    if (i == 0) {
                        if (phase == ExplorerPhase::Exploring) {
                            phase = ExplorerPhase::ChoosingMode;
                            selectedGameMode = -1;
                            navPath.clear();
                            currentNode = &root;
                            onBoardStateChanged();
                        }
                    } else {
                        int targetMoves = (i - 1) * 2 + 1;
                        if (targetMoves < (int)navPath.size())
                            undoToMoveCount(targetMoves);
                    }
                    break;
                }
            }

            if (!moveHistory.rows[i].black.empty()) {
                Rectangle blackRect = {(float)hr.colSplit, (float)ry, (float)(hr.histX + hr.histW - hr.colSplit), (float)theme.itemH};
                if (CheckCollisionPointRec(mouse, blackRect)) {
                    moveHistory.hoveredIdx = i;
                    moveHistory.hoveredCell = 1;
                    if (click) {
                        if (i == 0) {
                            if (!navPath.empty())
                                undoToMoveCount(0);
                        } else {
                            int targetMoves = (i - 1) * 2 + 2;
                            if (targetMoves < (int)navPath.size())
                                undoToMoveCount(targetMoves);
                        }
                        break;
                    }
                }
            }
        }
    }

    // Mouse wheel and scrollbar-drag scrolling are both handled internally
    // by GuiScrollPanel (called from ScrollArea::begin in onDrawOverlay) —
    // it owns scrollArea.scroll.y entirely. A second, independently
    // computed wheel handler here used to fight it every frame (both would
    // add their own delta to the same value on the same GetMouseWheelMove()
    // reading, then GuiScrollPanel's clamp would win), which is why drag
    // and wheel scrolling didn't work.

    // ── highlights / preview ──
    int mlHover = moveList.hoveredIdx;

    // Determine history hover ply (-2 = none, -1 = root, >=0 = ply)
    int histPly = -2;
    if (moveHistory.hoveredIdx >= 0 && moveHistory.hoveredCell >= 0) {
        if (moveHistory.hoveredIdx == 0 && phase == ExplorerPhase::Exploring && !navPath.empty()) {
            histPly = -1;  // root state
        } else if (moveHistory.hoveredIdx >= 1) {
            int ply = (moveHistory.hoveredIdx - 1) * 2 + moveHistory.hoveredCell;
            if (ply >= 0 && ply < (int)navPath.size())
                histPly = ply;
        }
    }

    // Mode choice hover
    bool modeChoiceHover = phase == ExplorerPhase::ChoosingMode && mlHover >= 0 && mlHover < 8;
    int previewKey = modeChoiceHover ? (1000 + mlHover) : (histPly >= -1 ? histPly : -2);

    // Preview transition
    if (previewKey != historyPreviewPly) {
        if (historyPreviewPly >= -2)
            boardView.clearHistoryPreview();

        if (previewKey >= -1) {
            auto replayToPly = [&](int target, int gm) -> GamePosition {
                ::Board b;
                std::bitset<3> set(gm);
                b.rst(set);
                std::string pos = rootPos;
                size_t space = pos.rfind(' ');
                if (space != std::string::npos)
                    pos = pos.substr(0, space + 1) + makeModeToken(gm);
                b.loadPosition(pos);
                for (int p = 0; p <= target; p++)
                    b.unsafeMakeMove(navPath[p].move);
                return b.positionCopy();
            };

            if (modeChoiceHover) {
                boardView.setHistoryPreview(replayToPly(-1, mlHover));
            } else if (histPly >= 0) {
                boardView.setHistoryPreview(replayToPly(histPly, selectedGameMode));
                historyPreviewInfo = navPath[histPly].info;
                historyPreviewInfo.xmove = {};
            } else {  // histPly == -1, root state
                boardView.setHistoryPreview(replayToPly(-1, selectedGameMode));
            }
        }
        historyPreviewPly = previewKey;
    }

    // Build highlights
    bool highlights[13][13]{};

    if (mlHover >= 0 && mlHover < (int)moveList.entries.size()) {
        auto* userData = moveList.entries[mlHover].userData;
        if (userData) {
            auto* md = static_cast<XMove*>(userData);
            if (md) {
                for (auto& tile : *md)
                    if (tile.i >= 0 && tile.i < 13 && tile.j >= 0 && tile.j < 13)
                        highlights[tile.i][tile.j] = true;
            }
        }
    }

    if (histPly >= 0 && histPly < (int)navPath.size()) {
        for (auto& tile : navPath[histPly].move)
            if (tile.i >= 0 && tile.i < 13 && tile.j >= 0 && tile.j < 13)
                highlights[tile.i][tile.j] = true;
    }

    boardView.setHighlights(highlights);

    // Set MoveInfo
    boardView.setMoveInfo(nullptr);
    if (mlHover >= 0 && mlHover < (int)moveList.entries.size()) {
        int origIdx = moveList.entries[mlHover].originalIndex;
        if (origIdx >= 0 && origIdx < (int)cachedKsnMoves.size())
            boardView.setMoveInfo(&cachedKsnMoves[origIdx].info);
    } else if (histPly >= 0) {
        boardView.setMoveInfo(&historyPreviewInfo);
    }
}

// ── onDraw ──────────────────────────────────────────────────────────

void OpeningExplorer::onDraw(Rectangle rect) {
    ui::Slot* boardSlot = layout.find("board");
    boardView.draw(boardSlot ? boardSlot->rect : rect, theme.scale);
}

// ── computeHitRects ──────────────────────────────────────────────────
// Shared by onTick (hit-testing) and onDrawOverlay (drawing) so the two
// never drift out of sync the way they previously could (each recomputed
// the same rects independently).

OpeningExplorer::HitRects OpeningExplorer::computeHitRects() {
    float s = theme.scale;
    ui::Slot* histSlot = layout.find("history");
    ui::Slot* movesSlot = layout.find("moves");
    ui::Slot* rightSlot = layout.find("right");

    HitRects hr{};
    hr.right = rightSlot ? rightSlot->rect : Rectangle{0, 0, 0, 0};
    int rightX = (int)hr.right.x;
    int rightW = (int)hr.right.width;

    hr.histX = rightX + (int)(12 * s);
    hr.histY = histSlot ? (int)histSlot->rect.y + (int)(28 * s) : 0;
    hr.histW = rightW - (int)(24 * s);
    int indent = (int)(12 * s);
    hr.colSplit = hr.histX + indent + (hr.histW - indent * 2) / 2;

    hr.histEnd = hr.histY + moveHistory.totalHeight(theme) + (int)(4 * s);

    hr.listHeaderY = hr.histEnd + (int)(32 * s);
    hr.listX = hr.histX;
    hr.listW = hr.histW;
    hr.listY = hr.listHeaderY + theme.fontSizeSmall + (int)(6 * s);
    hr.listAreaBot = movesSlot ? (int)(movesSlot->rect.y + movesSlot->rect.height) : hr.listY;

    return hr;
}

// ── onDrawOverlay ────────────────────────────────────────────────────

void OpeningExplorer::onDrawOverlay(Rectangle rect) {
    float s = theme.scale;
    float w = rect.width;
    float leftEdge = rect.x;

    HitRects hr = computeHitRects();

    ui::Panel::draw(hr.right, "", theme);

    DrawAppText("Moves played", (float)hr.histX, (float)hr.histY - 20 * s,
                (float)theme.fontSizeSmall, theme.textSection);
    moveHistory.draw(hr.histX, hr.histY, hr.histW, theme);

    DrawLine(hr.histX, hr.histEnd, hr.histX + hr.histW, hr.histEnd, theme.border);

    DrawAppText(getOpeningName().c_str(), (float)hr.histX, (float)(hr.histEnd + 8 * s),
                (float)theme.fontSizeTitle, theme.textAccent);

    DrawAppText("Next moves", (float)hr.histX, (float)hr.listHeaderY,
                (float)theme.fontSizeSmall, theme.textSection);
    float evalW = MeasureAppText("-0.00", (float)theme.fontSizeSmall);
    DrawAppText("Eval", (float)(hr.histX + hr.histW - evalW - 4 * s), (float)hr.listHeaderY,
                (float)theme.fontSizeSmall, theme.textDim);

    Rectangle scrollBounds = { (float)hr.listX, (float)hr.listY, (float)hr.listW,
                               (float)(hr.listAreaBot - hr.listY) };
    Rectangle view = scrollArea.begin(scrollBounds, moveList.totalHeight(theme));
    moveList.draw((int)view.x, (int)view.y + (int)scrollArea.scroll.y, (int)view.width, theme);
    scrollArea.end();

    // Analysis panel
    ui::Slot* analysisSlot = layout.find("analysis");
    analysis.draw(analysisSlot->rect, theme);

    // Top bar
    ui::Slot* topbarSlot = layout.find("topbar");
    DrawRectangleRec(topbarSlot->rect, Color{30, 30, 48, 255});
    DrawLine((int)leftEdge, (int)(topbarSlot->rect.y + topbarSlot->rect.height),
             (int)(leftEdge + w), (int)(topbarSlot->rect.y + topbarSlot->rect.height), theme.border);
    {
        std::string titleText = "Opening Explorer";
        DrawAppText(titleText.c_str(), leftEdge + 12 * s, topbarSlot->rect.y + 10 * s,
                    (float)theme.fontSizeTitle, theme.text);
        std::string openName = getOpeningName();
        float nameW = MeasureAppText(openName.c_str(), (float)theme.fontSizeTitle);
        DrawAppText(openName.c_str(),
                    (float)(leftEdge + w - 12 * s - nameW), topbarSlot->rect.y + 10 * s,
                    (float)theme.fontSizeTitle, theme.textAccent);
    }

    // Bottom bar
    ui::Slot* bottombarSlot = layout.find("bottombar");
    DrawRectangleRec(bottombarSlot->rect, Color{30, 30, 48, 255});
    DrawLine((int)leftEdge, (int)bottombarSlot->rect.y,
             (int)(leftEdge + w), (int)bottombarSlot->rect.y, theme.border);
    {
        std::string status;
        if (navPath.empty())
            status = "Start position";
        else
            status = std::to_string((int)navPath.size()) + " ply  |  "
                   + std::to_string((int)moveList.entries.size()) + " candidates  |  "
                   + (currentNode ? "book" : "off-book");
        DrawAppText(status.c_str(), leftEdge + 12 * s, bottombarSlot->rect.y + 6 * s,
                    (float)theme.fontSizeSmall, theme.textDim);
        std::string controls = "W/S: navigate  D: play  A: back  DEL: reset";
        float ctrlW = MeasureAppText(controls.c_str(), (float)theme.fontSizeSmall);
        DrawAppText(controls.c_str(),
                    (float)(leftEdge + w - 12 * s - ctrlW),
                    bottombarSlot->rect.y + 6 * s,
                    (float)theme.fontSizeSmall, theme.textDim);
    }
}
