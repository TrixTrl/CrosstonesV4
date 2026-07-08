#include "GameApp.h"
#include "raylib.h"
#include "app/FontManager.h"
#include "game-suite/ManualPlayer.h"
#include "game-suite/GameMaster.h"
#include "StackThread.h"
#include "data/GamePosition.h"
#include "felix-bots/Deepchad.h"
#include "trix-bots/TheFirst.h"
#include "trix-bots/Hydra.h"
#include "trix-bots/AlphaCruncher.h"
#include <bitset>
#include <cstring>
#include <string>
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

Player* GameApp::createPlayer(const std::string& type, int param, ManualPlayer** outManual) {
    *outManual = nullptr;
    if (type == "Deepchad") return new Deepchad(param);
    if (type == "AlphaCruncher") return new AlphaCruncher(param);
    if (type == "Hydra") return new Hydra(param, 14);
    if (type == "TheFirst") return new TheFirst(param);
    if (type == "ManualPlayer") {
        auto* mp = new ManualPlayer(&boardView.position().pieces);
        mp->onDisplayChanged = []{};
        *outManual = mp;
        return mp;
    }
    return new Deepchad(3);
}

static void drawRadio(Rectangle r, const char* label, bool selected, Vector2 mouse, const ui::Theme& t) {
    Color dot;
    if (selected) dot = {100, 200, 100, 255};
    else dot = {128, 128, 128, 255};
    bool hover = CheckCollisionPointRec(mouse, r);
    if (hover) {
        if (selected) dot = {140, 240, 140, 255};
        else dot = {180, 180, 180, 255};
    }
    float cx = r.x + 10 * t.scale;
    float cy = r.y + r.height / 2;
    DrawCircleV({cx, cy}, 6 * t.scale, dot);
    if (selected)
        DrawCircleV({cx, cy}, 3 * t.scale, WHITE);
    Color dim = {180, 180, 180, 255};
    DrawAppText(label, r.x + 22 * t.scale, r.y + 3 * t.scale, (float)t.fontSize, selected ? WHITE : dim);
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
    layout = { .dir = ui::Dir::Col, .children = {
        { .id = "title",   .fixed = 50  },
        { .id = "players", .fixed = 90, .dir = ui::Dir::Row, .children = {
            { .id = "p1slot", .flex = 1 },
            { .id = "p2slot", .flex = 1 },
        }},
        { .id = "posarea", .fixed = 120 },
        { .id = "startbtn",.fixed = 50  },
    }};

    setup.playerTypes = {"Deepchad", "AlphaCruncher", "Hydra", "TheFirst", "ManualPlayer"};
    setup.p1Field.text = "3";
    setup.p1Field.commit();
    setup.p2Field.text = "3";
    setup.p2Field.commit();
}

// ── onDraw ──

void GameApp::onDraw(Rectangle rect) {
    if (phase == SETUP) {
        drawSetupForm(rect);
        return;
    }
    resolveLayout(rect);
    auto* boardSlot = layout.find("board");
    boardView.draw(boardSlot ? boardSlot->rect : rect, theme.scale);
}

void GameApp::drawSetupForm(Rectangle rect) {
    resolveLayout(rect);
    float s = theme.scale;
    int itemH = theme.itemH;
    int fs = theme.fontSize;
    int fsTitle = theme.fontSizeTitle;
    Vector2 mouse = GetMousePosition();
    bool click = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
    auto& setup = this->setup;

    std::vector<std::string_view> dropdownNames;
    int dropdownCount = 0;
    buildDropdownForSource(setup.posSource, setup.gameEntries, dropdownNames, dropdownCount);

    bool blocked = setup.mainDropdown.open || setup.p1Field.active || setup.p2Field.active;

    auto* playersSlot = layout.find("players");
    auto* posSlot = layout.find("posarea");
    auto* startSlot = layout.find("startbtn");

    const float startX = rect.x + 50 * s;
    const float pY = playersSlot ? playersSlot->rect.y : (rect.y + 140 * s);
    const float posY = posSlot ? posSlot->rect.y : (rect.y + 260 * s);
    const float startBtnY = startSlot ? startSlot->rect.y : (rect.y + 380 * s);

    Rectangle p1StepperR = {startX + 35 * s, pY + 28 * s, 230 * s, (float)itemH};
    Rectangle p2StepperR = {startX + 415 * s, pY + 28 * s, 230 * s, (float)itemH};
    Rectangle p1InputR   = {startX + 40 * s, pY + 63 * s, 100 * s, (float)itemH};
    Rectangle p2InputR   = {startX + 420 * s, pY + 63 * s, 100 * s, (float)itemH};

    Rectangle startPosR  = {startX, posY + 25 * s, 130 * s, 22 * s};
    Rectangle presetR    = {startX + 140 * s, posY + 25 * s, 80 * s, 22 * s};
    Rectangle savedR     = {startX + 230 * s, posY + 25 * s, 80 * s, 22 * s};
    Rectangle mainDropdownR = {startX, posY + 60 * s, 300 * s, (float)(itemH - 2)};

    // ── Input ──
    if (!blocked && click) {
        auto hit = [&](Rectangle r) { return CheckCollisionPointRec(mouse, r); };
        if (hit(startPosR) && setup.posSource != POS_START) {
            setup.posSource = POS_START;
            setup.mainDropdown.selectedIdx = setup.lastGmIdx;
            setup.mainDropdown.open = false;
            buildDropdownForSource(setup.posSource, setup.gameEntries, dropdownNames, dropdownCount);
        } else if (hit(presetR) && setup.posSource != POS_PRESET) {
            setup.posSource = POS_PRESET;
            setup.gameEntries = GameEntries::loadAll();
            buildDropdownForSource(setup.posSource, setup.gameEntries, dropdownNames, dropdownCount);
            setup.mainDropdown.selectedIdx = (std::min)(setup.presetIdx, (std::max)(0, dropdownCount - 1));
            setup.mainDropdown.open = false;
        } else if (hit(savedR) && setup.posSource != POS_SAVED) {
            setup.posSource = POS_SAVED;
            setup.gameEntries = GameEntries::loadAll();
            buildDropdownForSource(setup.posSource, setup.gameEntries, dropdownNames, dropdownCount);
            setup.mainDropdown.selectedIdx = (std::min)(setup.saveIdx, (std::max)(0, dropdownCount - 1));
            setup.mainDropdown.open = false;
        }
    }

    int ddSel = setup.mainDropdown.handleInput(mainDropdownR, mouse, click, dropdownCount);
    if (ddSel >= 0) {
        setup.mainDropdown.selectedIdx = ddSel;
        if (setup.posSource == POS_START) setup.lastGmIdx = ddSel;
        else if (setup.posSource == POS_PRESET) setup.presetIdx = ddSel;
        else setup.saveIdx = ddSel;
    }

    if (!blocked) {
        setup.p1Stepper.handleInput(p1StepperR, mouse, click, (int)setup.playerTypes.size());
        setup.p2Stepper.handleInput(p2StepperR, mouse, click, (int)setup.playerTypes.size());
    }
    setup.p1Field.handleInput(p1InputR, mouse, click);
    setup.p2Field.handleInput(p2InputR, mouse, click);

    Rectangle startBtnR = {startX, startBtnY + 5 * s, 160 * s, 40 * s};
    bool startHover = CheckCollisionPointRec(mouse, startBtnR);
    if (!blocked && click && startHover) {
        Parameters& p = params;
        p.player1Type = setup.playerTypes[setup.p1Stepper.selectedIdx];
        p.player1Param = setup.p1Field.text;
        p.player2Type = setup.playerTypes[setup.p2Stepper.selectedIdx];
        p.player2Param = setup.p2Field.text;
        p.gameModeBits = (setup.posSource == POS_START) ? setup.mainDropdown.selectedIdx : setup.lastGmIdx;

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
        p1 = createPlayer(p.player1Type, p1p, &manualP1);
        p2 = createPlayer(p.player2Type, p2p, &manualP2);

        gameMaster = std::make_unique<GameMaster>(
            gamemode, p1, p2, 10000, 0, boardView.position()
        );

        if (!loadedPosition.position.empty())
            gameMaster->loadPos(loadedPosition.position);

        prevBoard = boardView.position();
        std::memset(lastMoveHighlights, 0, sizeof(lastMoveHighlights));

        gameThread = std::make_unique<StackThread>([this]() {
            gameMaster->play(stopSource.get_token(), []{}, true);
        });

        return;
    }

    // ── Draw ──
    Color dim = {128, 128, 128, 255};
    std::vector<std::string_view> typeOpts;
    for (auto& t : setup.playerTypes) typeOpts.push_back(t);

    auto* titleSlot = layout.find("title");
    DrawAppText("Game Setup", titleSlot ? (titleSlot->rect.x + 50 * s) : (rect.x + 50 * s),
                titleSlot ? (titleSlot->rect.y + 10 * s) : (rect.y + 20 * s),
                (float)(fsTitle * 1.5f), WHITE);

    DrawAppText("Player 1 (White):", startX, pY + 5 * s, (float)fsTitle, WHITE);
    setup.p1Stepper.draw(p1StepperR, typeOpts, theme);
    setup.p1Field.draw(p1InputR, theme);
    DrawAppText("depth/time", startX + 150 * s, pY + 71 * s, (float)fs, dim);

    DrawAppText("Player 2 (Black):", startX + 380 * s, pY + 5 * s, (float)fsTitle, WHITE);
    setup.p2Stepper.draw(p2StepperR, typeOpts, theme);
    setup.p2Field.draw(p2InputR, theme);
    DrawAppText("depth/time", startX + 530 * s, pY + 71 * s, (float)fs, dim);

    DrawAppText("Position:", startX, posY + 5 * s, (float)fsTitle, WHITE);
    drawRadio(startPosR, "Start position", setup.posSource == POS_START, mouse, theme);
    drawRadio(presetR, "Preset", setup.posSource == POS_PRESET, mouse, theme);
    drawRadio(savedR, "Saved", setup.posSource == POS_SAVED, mouse, theme);

    if (dropdownCount == 0 && setup.posSource != POS_START) {
        float tw = MeasureAppText("(none available)", (float)fs);
        DrawAppText("(none available)", mainDropdownR.x + (mainDropdownR.width - tw) / 2,
                    mainDropdownR.y + 6 * s, (float)fs, dim);
    } else {
        setup.mainDropdown.draw(mainDropdownR, dropdownNames, theme);
    }

    bool startHoverD = CheckCollisionPointRec(mouse, startBtnR);
    Color btnColor = startHoverD ? Color{60, 60, 90, 255} : Color{45, 45, 65, 255};
    DrawRectangleRec(startBtnR, btnColor);
    DrawRectangleLinesEx(startBtnR, (int)(1 * s), Color{80, 80, 120, 255});
    float tw = MeasureAppText("Start Game", (float)fsTitle);
    DrawAppText("Start Game", startBtnR.x + (startBtnR.width - tw) / 2,
                startBtnR.y + (startBtnR.height - (float)fsTitle) / 2, (float)fsTitle, WHITE);
}

void GameApp::onTick(float dt) {
    if (phase == SETUP) return;

    // Merge highlights
    bool merged[13][13]{};
    for (int x = 0; x < 13; x++)
        for (int y = 0; y < 13; y++) {
            bool mh = (manualP1 && manualP1->cellHighlights[x][y]) ||
                       (manualP2 && manualP2->cellHighlights[x][y]);
            merged[x][y] = lastMoveHighlights[x][y] || mh;
        }

    boardView.setHighlights(merged);
    boardView.setMoveInfo(nullptr);
}

void GameApp::onDrawOverlay(Rectangle rect) {
    float s = theme.scale;
    if (phase == SETUP) return;

    // Save button
    Rectangle btnRect = {rect.x + rect.width - 120 * s, rect.y + 8 * s, 110 * s, 28 * s};
    bool hover = CheckCollisionPointRec(GetMousePosition(), btnRect);
    bool clicked = hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);

    Color btnColor = hover ? Color{80, 80, 120, 255} : Color{55, 55, 80, 255};
    DrawRectangleRec(btnRect, btnColor);
    DrawRectangleLinesEx(btnRect, (int)(1 * s), Color{100, 100, 150, 255});
    float tw = MeasureAppText("Save Position", (float)theme.fontSize);
    DrawAppText("Save Position", btnRect.x + (btnRect.width - tw) / 2,
                btnRect.y + 7 * s, (float)theme.fontSize, WHITE);

    if (clicked || IsKeyPressed(KEY_F5)) {
        if (gameMaster) {
            auto& record = gameMaster->getBoard().gameRecord;
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
            saveFlashTimer = 120;
        }
    }

    if (saveFlashTimer > 0) {
        float alpha = (std::min)(saveFlashTimer / 60.0f, 1.0f);
        Color flashColor = {100, 200, 100, (unsigned char)(alpha * 255)};
        Rectangle flashRect = {rect.x + rect.width - 120 * s, rect.y + 40 * s, 110 * s, 22 * s};
        DrawRectangleRec(flashRect, {30, 30, 50, (unsigned char)(alpha * 200)});
        DrawAppText("Saved!", flashRect.x + 10 * s, flashRect.y + 4 * s, (float)theme.fontSize, flashColor);
        saveFlashTimer--;
    }
}

void GameApp::onStop() {
    stopSource.request_stop();
    if (gameThread && gameThread->joinable())
        gameThread->join();

    delete p1;
    delete p2;
}
