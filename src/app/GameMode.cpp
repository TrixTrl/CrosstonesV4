#include "GameMode.h"
#include "BoardDraw.h"
#include "FontManager.h"
#include "game-suite/GameMaster.h"
#include "game-suite/GamePosition.h"
#include "game-suite/ManualPlayer.h"
#include "globals/Player.h"
#include "globals/Piece.h"
#include "felix-bots/Deepchad.h"
#include "trix-bots/TheFirst.h"
#include "trix-bots/Hydra.h"
#include "trix-bots/AlphaCruncher.h"
#include <string>
#include <bitset>
#include <thread>
#include <cstring>

static Player* createPlayer(const std::string& type, int param, uint8_t (*displayBoard)[13][13],
                            std::function<void()> onDisplayChanged, ManualPlayer** outManual) {
    *outManual = nullptr;

    if (type == "Deepchad") {
        return new Deepchad(param);
    } else if (type == "AlphaCruncher") {
        return new AlphaCruncher(param);
    } else if (type == "Hydra") {
        return new Hydra(param, 14);
    } else if (type == "TheFirst") {
        return new TheFirst(param);
    } else if (type == "ManualPlayer") {
        ManualPlayer* mp = new ManualPlayer(displayBoard);
        mp->onDisplayChanged = onDisplayChanged;
        *outManual = mp;
        return mp;
    }

    // Default
    return new Deepchad(param);
}

void GameMode::run(const AppConfig& config) {
    InitWindow(13 * cellSize, 13 * cellSize, "Crosstones V4");
    SetTargetFPS(60);
    initAppFont();

    GamePosition displayBoard = {0};
    std::bitset<3> gamemode(config.game.gameModeBits);

    // --- Preserved commented player toggles ---
    // Player *p1 = new TheFirst(2);
    // Player *p1 = new Hydra(4, 14);
    // Player *p1 = new ManualPlayer(&ui, globalHwnd, &displayBoard[0]);
    // Player *p1 = new AlphaCruncher(20);
    // Player *p2 = new TheFirst(2);
    // Player *p2 = new Hydra(4, 14);
    // Player *p2 = new Deepchad(4);
    // Player *p2 = new AlphaCruncher(60);

    ManualPlayer* manualP1 = nullptr;
    ManualPlayer* manualP2 = nullptr;

    Player* p1 = createPlayer(config.game.player1.type, config.game.player1.param,
                              &displayBoard.pieces, [&]() {}, &manualP1);
    Player* p2 = createPlayer(config.game.player2.type, config.game.player2.param,
                              &displayBoard.pieces, [&]() {}, &manualP2);

    GameMaster gameMaster(gamemode, p1, p2, config.game.timeControl, config.game.enforceTime, displayBoard);

    // --- Preserved commented starting positions ---
    // gameMaster.loadPos("b-10000 b-10012 -B30404 -W30512 -B30602 rW30608 bB50610 -W30712 rW40808 b-11200 b-11212 11111111100111111111");
    // gameMaster.loadPos("b-10000 b-10012 -B30404 -W30512 -B30602 rW50607 bB50610 -W30712 rW20808 b-11200 b-11212 11111111100111111111");
    // gameMaster.loadPos("-W10006 -B11206 11010100001100111111");
    // gameMaster.loadPos("bW30404 -B10600 -B10800 rW30804 11010100001100111111");
    // gameMaster.loadPos("b-10000 b-10012 rW30406 -B30500 -B30502 -W10512 rB20610 bB30611 -B30700 -W30712 b-11200 b-11212 11110011111111101111");
    // gameMaster.loadPos("b-10000 r-10006 b-10012 -W30408 -B30500 -B30502 -W30512 b-10606 -B30700 -B30702 -W30710 -W30712 b-11200 r-11206 b-11212 11111111111111111111");
    // gameMaster.loadPos("b-10000 r-10006 b-10012 -B10500 -B10502 -W10510 -W10512 b-10606 -B10700 -B10702 -W10710 -W10712 b-11200 r-11206 b-11212 11111111111111111111");
    // gameMaster.loadPos("b-10000 r-10006 b-10012 -W10409 -B10502 bW20603 -W20710 -B10803 -B10900 b-11200 r-11206 b-11212 11111111111111111111");
    // gameMaster.loadPos("-B10500 -W10602 11111111111111111111");
    // gameMaster.loadPos("b-10000 r-10006 b-10012 -W30510 -B30502 b-10606 -B30702 -W30710 b-11200 r-11206 b-11212 11111111111111111111"); // slaughterhouse
    // gameMaster.loadPos("b-10000 r-10006 b-10012 -W30609 -B30603 b-11200 r-11206 b-11212 11111111111111111111");
    // gameMaster.loadPos("b-10002 b-10005 -B10102 -W20308 rW30404 -B10500 -B10512 -B10602 -W20604 b-10607 r-10608 -W30610 -B10700 b-10708 -W10804 -W10912 -B11100 -B11110 11111100101101111111");
    // gameMaster.loadPos("r-10006 bW40112 -B30202 -B20300 -B10302 bB20400 -B10402 -W30411 -W20508 bW20606 -W30610 -B20801 -B20902 b-11200 r-11206 b-11212 00000000000100000000");

    if (!config.game.position.empty()) {
        gameMaster.loadPos(config.game.position);
    }

    // Track previous board state for bot move highlighting
    GamePosition prevBoard = displayBoard;
    bool lastMoveHighlights[13][13] = {false};

    std::thread gameThread([&]() {
        gameMaster.play(std::stop_token{}, [&]() {
            // Board changed - raylib loop picks it up automatically
        }, true);
    });

    while (!WindowShouldClose()) {
        bool highlights[13][13] = {false};

        // Detect board changes and persist highlights until next move
        bool boardChanged = false;
        for (int x = 0; x < 13 && !boardChanged; x++)
            for (int y = 0; y < 13 && !boardChanged; y++)
                if (displayBoard.pieces[x][y] != prevBoard.pieces[x][y])
                    boardChanged = true;

        if (boardChanged) {
            for (int x = 0; x < 13; x++)
                for (int y = 0; y < 13; y++)
                    lastMoveHighlights[x][y] = displayBoard.pieces[x][y] != prevBoard.pieces[x][y];
            std::memcpy(&prevBoard.pieces, &displayBoard.pieces, sizeof(displayBoard.pieces));
        }

        // Draw last-move highlights
        for (int x = 0; x < 13; x++)
            for (int y = 0; y < 13; y++)
                if (lastMoveHighlights[x][y])
                    highlights[x][y] = true;

        // Superimpose ManualPlayer highlights (transient hover/selection state)
        if (manualP1) {
            for (int x = 0; x < 13; x++)
                for (int y = 0; y < 13; y++)
                    if (manualP1->cellHighlights[x][y])
                        highlights[x][y] = true;
        }
        if (manualP2) {
            for (int x = 0; x < 13; x++)
                for (int y = 0; y < 13; y++)
                    if (manualP2->cellHighlights[x][y])
                        highlights[x][y] = true;
        }

        BeginDrawing();
        drawBoard(&displayBoard.pieces, highlights);
        EndDrawing();
    }

    cleanupAppFont();
    gameThread.detach();
    CloseWindow();

    delete p1;
    delete p2;
}
