#include "GameApp.h"
#include "raylib.h"
#include "app/FontManager.h"
#include "app/services/player_factory.h"
#include "game-suite/ManualPlayer.h"
#include "game-suite/GameMaster.h"
#include "StackThread.h"
#include "data/GamePosition.h"
#include "app/persistence/GameSetupConfig.h"
#include <string>
#include <bitset>
#include <cstring>
#include <algorithm>

GameApp::GameApp() = default;
GameApp::~GameApp() = default;

static const char* timestampStr() {
    time_t t = time(nullptr);
    struct tm lt;
    localtime_s(&lt, &t);
    static char buf[32];
    strftime(buf, sizeof(buf), "%Y%m%d-%H%M%S", &lt);
    return buf;
}

static const char* gameModeNames[] = {
    "Phoenix", "Monkey", "Lotus", "Kylin",
    "Tortoise", "Orchid", "Crane", "Dragon"
};

static void buildDropdownForSource(GameApp::PosSource src, GameEntries& entries,
                                   std::vector<std::string_view>& names, int& count) {
    names.clear(); count = 0;
    if (src == GameApp::POS_START) {
        for (int p = 0; p < 8; p++) names.push_back(gameModeNames[p]);
        count = 8;
    } else if (src == GameApp::POS_PRESET) {
        for (auto& e : entries.presets) names.push_back(e.name);
        count = (int)entries.presets.size();
    } else {
        for (auto& e : entries.saves) names.push_back(e.name);
        count = (int)entries.saves.size();
    }
}

// ── onStart ──

void GameApp::onStart() {
    layout = { .dir = ui::Dir::Col, .padding = {0, 50, 0, 50}, .children = {
        { .id = "title",   .fixed = 50  },
        { .id = "players", .fixed = 90, .dir = ui::Dir::Row, .gap = 30, .children = {
            { .id = "p1slot", .flex = 1 },
            { .id = "p2slot", .flex = 1 },
        }},
        { .id = "posarea", .fixed = 120 },
        { .id = "startbtn",.fixed = 50  },
    }};

    setup.playerTypes.clear();
    for (auto& t : services::availablePlayerTypes()) setup.playerTypes.push_back(t.id);

    GameSetupConfig cfg = GameSetupConfig::load();
    auto findType = [&](const std::string& type, int fallback) {
        for (int i = 0; i < (int)setup.playerTypes.size(); i++)
            if (setup.playerTypes[i] == type) return i;
        return fallback;
    };

    setup.p1Stepper.selectedIdx = findType(cfg.player1Type, 0);
    setup.p1Field.text = cfg.player1Param;
    setup.p1Field.commit();

    setup.p2Stepper.selectedIdx = findType(cfg.player2Type, (std::min)(1, (int)setup.playerTypes.size() - 1));
    setup.p2Field.text = cfg.player2Param;
    setup.p2Field.commit();

    setup.lastGmIdx = cfg.gameModeBits;
    setup.mainDropdown.selectedIdx = cfg.gameModeBits;
}

// ── layout helper (shared by onTickSetup and drawSetupForm — computed once
// per call site rather than duplicated, to avoid the desync risk of two
// independently-maintained copies of the same rect math) ──

GameApp::SetupRects GameApp::computeSetupRects() {
    float s = theme.scale;
    int itemH = theme.itemH;

    auto* p1Slot = layout.find("p1slot");
    auto* p2Slot = layout.find("p2slot");
    auto* posSlot = layout.find("posarea");
    auto* startSlot = layout.find("startbtn");

    Rectangle p1r = p1Slot ? p1Slot->rect : Rectangle{0, 0, 0, 0};
    Rectangle p2r = p2Slot ? p2Slot->rect : Rectangle{0, 0, 0, 0};
    Rectangle posr = posSlot ? posSlot->rect : Rectangle{0, 0, 0, 0};
    Rectangle startr = startSlot ? startSlot->rect : Rectangle{0, 0, 0, 0};

    SetupRects r;
    r.p1Stepper = {p1r.x + 35 * s, p1r.y + 28 * s, 230 * s, (float)itemH};
    r.p1Input   = {p1r.x + 40 * s, p1r.y + 63 * s, 100 * s, (float)itemH};
    r.p2Stepper = {p2r.x + 35 * s, p2r.y + 28 * s, 230 * s, (float)itemH};
    r.p2Input   = {p2r.x + 40 * s, p2r.y + 63 * s, 100 * s, (float)itemH};

    r.startPos     = {posr.x, posr.y + 25 * s, 130 * s, 22 * s};
    r.preset       = {posr.x + 140 * s, posr.y + 25 * s, 80 * s, 22 * s};
    r.saved        = {posr.x + 230 * s, posr.y + 25 * s, 80 * s, 22 * s};
    r.mainDropdown = {posr.x, posr.y + 60 * s, 300 * s, (float)(itemH - 2)};

    r.startBtn = {startr.x, startr.y + 5 * s, 160 * s, 40 * s};
    return r;
}

// ── onTick ──

void GameApp::onTick(float dt, const InputState& input) {
    if (phase == SETUP) onTickSetup(input);
    else onTickPlaying(input);
}

void GameApp::onTickSetup(const InputState& input) {
    Vector2 mouse = input.mouse;
    bool click = input.clicked;

    std::vector<std::string_view> dropdownNames;
    int dropdownCount = 0;
    buildDropdownForSource(setup.posSource, setup.gameEntries, dropdownNames, dropdownCount);

    bool blocked = setup.mainDropdown.open || setup.p1Field.active || setup.p2Field.active;
    SetupRects r = computeSetupRects();

    ui::RadioOption posOptions[] = {
        {r.startPos, "Start position"},
        {r.preset,   "Preset"},
        {r.saved,    "Saved"},
    };

    if (!blocked) {
        int radioSel = setup.posRadio.handleInput(posOptions, mouse, click);
        if (radioSel == 0 && setup.posSource != POS_START) {
            setup.posSource = POS_START;
            setup.mainDropdown.selectedIdx = setup.lastGmIdx;
            setup.mainDropdown.open = false;
        } else if (radioSel == 1 && setup.posSource != POS_PRESET) {
            setup.posSource = POS_PRESET;
            setup.gameEntries = GameEntries::loadAll();
            buildDropdownForSource(setup.posSource, setup.gameEntries, dropdownNames, dropdownCount);
            setup.mainDropdown.selectedIdx = (std::min)(setup.presetIdx, (std::max)(0, dropdownCount - 1));
            setup.mainDropdown.open = false;
        } else if (radioSel == 2 && setup.posSource != POS_SAVED) {
            setup.posSource = POS_SAVED;
            setup.gameEntries = GameEntries::loadAll();
            buildDropdownForSource(setup.posSource, setup.gameEntries, dropdownNames, dropdownCount);
            setup.mainDropdown.selectedIdx = (std::min)(setup.saveIdx, (std::max)(0, dropdownCount - 1));
            setup.mainDropdown.open = false;
        }
    } else {
        setup.posRadio.hoveredIdx = -1;
    }

    int ddSel = setup.mainDropdown.handleInput(r.mainDropdown, mouse, click, dropdownCount);
    if (ddSel >= 0) {
        setup.mainDropdown.selectedIdx = ddSel;
        if (setup.posSource == POS_START) setup.lastGmIdx = ddSel;
        else if (setup.posSource == POS_PRESET) setup.presetIdx = ddSel;
        else setup.saveIdx = ddSel;
    }

    if (!blocked) {
        setup.p1Stepper.handleInput(r.p1Stepper, mouse, click, (int)setup.playerTypes.size());
        setup.p2Stepper.handleInput(r.p2Stepper, mouse, click, (int)setup.playerTypes.size());
    }
    setup.p1Field.handleInput(r.p1Input, mouse, click);
    setup.p2Field.handleInput(r.p2Input, mouse, click);

    bool startClicked = setup.startButton.handleInput(r.startBtn, mouse, click);
    if (!blocked && startClicked) {
        startGame();
    }
}

void GameApp::startGame() {
    Parameters& p = params;
    p.player1Type = setup.playerTypes[setup.p1Stepper.selectedIdx];
    p.player1Param = setup.p1Field.text;
    p.player2Type = setup.playerTypes[setup.p2Stepper.selectedIdx];
    p.player2Param = setup.p2Field.text;
    p.gameModeBits = (setup.posSource == POS_START) ? setup.mainDropdown.selectedIdx : setup.lastGmIdx;

    GameSetupConfig cfg;
    cfg.player1Type = p.player1Type;
    cfg.player1Param = p.player1Param;
    cfg.player2Type = p.player2Type;
    cfg.player2Param = p.player2Param;
    cfg.gameModeBits = p.gameModeBits;
    cfg.save();

    if (setup.posSource == POS_PRESET || setup.posSource == POS_SAVED) {
        auto& src = (setup.posSource == POS_PRESET) ? setup.gameEntries.presets : setup.gameEntries.saves;
        int idx = (setup.posSource == POS_PRESET) ? setup.presetIdx : setup.saveIdx;
        if (idx >= 0 && idx < (int)src.size()) {
            auto& entry = src[idx];
            loadedPosition.position = entry.position.empty() ? setup.gameEntries.rootPos : entry.position;
            loadedPosition.moveHistory = entry.moves;
            loadedPosition.displayName = entry.name;
        }
    }

    phase = PLAYING;
    layout = { .dir = ui::Dir::Col, .children = {
        { .id = "board", .flex = 1 },
    }};

    std::bitset<3> gamemode(p.gameModeBits);
    int p1p = p.player1Param.empty() ? 4 : std::stoi(p.player1Param);
    int p2p = p.player2Param.empty() ? 4 : std::stoi(p.player2Param);
    play.p1 = services::createPlayer(p.player1Type, p1p, &boardView.position(), &play.manualP1);
    play.p2 = services::createPlayer(p.player2Type, p2p, &boardView.position(), &play.manualP2);

    play.gameMaster = std::make_unique<GameMaster>(
        gamemode, play.p1, play.p2, 10000, 0, boardView.position()
    );

    if (!loadedPosition.position.empty())
        play.gameMaster->loadPos(loadedPosition.position);

    play.prevBoard = boardView.position();
    std::memset(play.lastMoveHighlights, 0, sizeof(play.lastMoveHighlights));

    play.gameThread = std::make_unique<StackThread>([this]() {
        play.gameMaster->play(play.stopSource.get_token(), []{}, true);
    });
}

void GameApp::onTickPlaying(const InputState& input) {
    // Merge highlights
    bool merged[13][13]{};
    for (int x = 0; x < 13; x++)
        for (int y = 0; y < 13; y++) {
            bool mh = (play.manualP1 && play.manualP1->cellHighlights[x][y]) ||
                       (play.manualP2 && play.manualP2->cellHighlights[x][y]);
            merged[x][y] = play.lastMoveHighlights[x][y] || mh;
        }

    boardView.setHighlights(merged);
    boardView.setMoveInfo(nullptr);

    // Save button hover/click (drawn in onDrawOverlay)
    float s = theme.scale;
    Rectangle contentRect = layout.rect;
    Rectangle btnRect = {contentRect.x + contentRect.width - 120 * s, contentRect.y + 8 * s, 110 * s, 28 * s};
    bool clicked = play.saveButton.handleInput(btnRect, input.mouse, input.clicked);

    if (clicked || IsKeyPressed(KEY_F5)) {
        if (play.gameMaster) {
            auto& record = play.gameMaster->getBoard().gameRecord;
            std::string name;
            if (params.player1Type == "ManualPlayer" && params.player2Type == "ManualPlayer") {
                name = std::string("Human Game ") + timestampStr();
            } else {
                name = params.player1Type + " v " + params.player2Type + " " + timestampStr() + " step " +
                       std::to_string((std::max)(0, (int)record.size() - 1));
            }

            std::vector<int> moves;
            for (size_t i = 1; i < record.size(); i++) {
                int idx = std::stoi(record[i]);
                moves.push_back(idx);
            }

            GameEntry entry;
            entry.name = name;
            entry.player1 = params.player1Type;
            entry.player2 = params.player2Type;
            entry.moves = moves;
            GameEntries::appendSave(entry);
            play.saveFlashTimer = 120;
        }
    }

    if (play.saveFlashTimer > 0) play.saveFlashTimer--;
}

// ── onDraw ──

void GameApp::onDraw(Rectangle rect) {
    if (phase == SETUP) {
        drawSetupForm(rect);
        return;
    }
    auto* boardSlot = layout.find("board");
    boardView.draw(boardSlot ? boardSlot->rect : rect, theme.scale);
}

void GameApp::drawSetupForm(Rectangle rect) {
    float s = theme.scale;
    int fs = theme.fontSize;
    int fsTitle = theme.fontSizeTitle;

    std::vector<std::string_view> dropdownNames;
    int dropdownCount = 0;
    buildDropdownForSource(setup.posSource, setup.gameEntries, dropdownNames, dropdownCount);

    SetupRects r = computeSetupRects();

    Color dim = {128, 128, 128, 255};
    std::vector<std::string_view> typeOpts;
    for (auto& t : setup.playerTypes) typeOpts.push_back(t);

    auto* titleSlot = layout.find("title");
    Rectangle titleR = titleSlot ? titleSlot->rect : rect;
    DrawAppText("Game Setup", titleR.x, titleR.y + 10 * s, (float)(fsTitle * 1.5f), WHITE);

    auto* p1Slot = layout.find("p1slot");
    auto* p2Slot = layout.find("p2slot");
    Rectangle p1r = p1Slot ? p1Slot->rect : rect;
    Rectangle p2r = p2Slot ? p2Slot->rect : rect;

    DrawAppText("Player 1 (White):", p1r.x, p1r.y + 5 * s, (float)fsTitle, WHITE);
    setup.p1Stepper.draw(r.p1Stepper, typeOpts, theme);
    setup.p1Field.draw(r.p1Input, theme);
    DrawAppText("depth/time", p1r.x + 150 * s, p1r.y + 71 * s, (float)fs, dim);

    DrawAppText("Player 2 (Black):", p2r.x, p2r.y + 5 * s, (float)fsTitle, WHITE);
    setup.p2Stepper.draw(r.p2Stepper, typeOpts, theme);
    setup.p2Field.draw(r.p2Input, theme);
    DrawAppText("depth/time", p2r.x + 150 * s, p2r.y + 71 * s, (float)fs, dim);

    auto* posSlot = layout.find("posarea");
    Rectangle posr = posSlot ? posSlot->rect : rect;
    DrawAppText("Position:", posr.x, posr.y + 5 * s, (float)fsTitle, WHITE);

    ui::RadioOption posOptions[] = {
        {r.startPos, "Start position"},
        {r.preset,   "Preset"},
        {r.saved,    "Saved"},
    };
    setup.posRadio.draw(posOptions, (int)setup.posSource, theme);

    if (dropdownCount == 0 && setup.posSource != POS_START) {
        float tw = MeasureAppText("(none available)", (float)fs);
        DrawAppText("(none available)", r.mainDropdown.x + (r.mainDropdown.width - tw) / 2,
                    r.mainDropdown.y + 6 * s, (float)fs, dim);
    } else {
        setup.mainDropdown.draw(r.mainDropdown, dropdownNames, theme);
    }

    setup.startButton.draw(r.startBtn, "Start Game", theme);
}

void GameApp::onDrawOverlay(Rectangle rect) {
    float s = theme.scale;
    if (phase == SETUP) return;

    // Save button
    Rectangle btnRect = {rect.x + rect.width - 120 * s, rect.y + 8 * s, 110 * s, 28 * s};
    play.saveButton.draw(btnRect, "Save Position", theme);

    if (play.saveFlashTimer > 0) {
        float alpha = (std::min)(play.saveFlashTimer / 60.0f, 1.0f);
        Color flashColor = {theme.success.r, theme.success.g, theme.success.b, (unsigned char)(alpha * 255)};
        Rectangle flashRect = {rect.x + rect.width - 120 * s, rect.y + 40 * s, 110 * s, 22 * s};
        DrawRectangleRec(flashRect, {30, 30, 50, (unsigned char)(alpha * 200)});
        DrawAppText("Saved!", flashRect.x + 10 * s, flashRect.y + 4 * s, (float)theme.fontSize, flashColor);
    }
}

void GameApp::onStop() {
    play.stopSource.request_stop();
    if (play.gameThread && play.gameThread->joinable())
        play.gameThread->join();

    delete play.p1;
    delete play.p2;
}
