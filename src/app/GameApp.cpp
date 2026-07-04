#include "GameApp.h"
#include "raylib.h"
#include "game-suite/GameMaster.h"
#include "game-suite/ManualPlayer.h"
#include "globals/Player.h"
#include "globals/Piece.h"
#include "felix-bots/Deepchad.h"
#include "trix-bots/bot-code/TheFirst.h"
#include "trix-bots/bot-code/Hydra.h"
#include "trix-bots/bot-code/AlphaCruncher.h"
#include <string>
#include <bitset>
#include <cstring>

GameApp::~GameApp() = default;

Player* GameApp::createPlayer(const std::string& type, int param,
                               ManualPlayer** outManual) {
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
        ManualPlayer* mp = new ManualPlayer(&displayBoard.pieces);
        mp->onDisplayChanged = []{};
        *outManual = mp;
        return mp;
    }

    return new Deepchad(param);
}

void GameApp::onStart() {
    std::bitset<3> gamemode(config->game.gameModeBits);

    p1 = createPlayer(config->game.player1.type, config->game.player1.param, &manualP1);
    p2 = createPlayer(config->game.player2.type, config->game.player2.param, &manualP2);

    gameMaster = std::make_unique<GameMaster>(
        gamemode, p1, p2, config->game.timeControl,
        config->game.enforceTime, displayBoard
    );

    if (!config->game.position.empty()) {
        gameMaster->loadPos(config->game.position);
    }

    prevBoard = displayBoard;
    std::memset(lastMoveHighlights, 0, sizeof(lastMoveHighlights));

    gameThread = std::make_unique<StackThread>([this]() {
        gameMaster->play(stopSource.get_token(), []{}, true);
    });
}

void GameApp::onFrame() {
    std::memset(highlights, 0, sizeof(highlights));

    bool boardChanged = false;
    for (int x = 0; x < 13 && !boardChanged; x++)
        for (int y = 0; y < 13 && !boardChanged; y++)
            if (displayBoard[x][y] != prevBoard[x][y])
                boardChanged = true;

    if (boardChanged) {
        for (int x = 0; x < 13; x++)
            for (int y = 0; y < 13; y++)
                lastMoveHighlights[x][y] = displayBoard[x][y] != prevBoard[x][y];
        prevBoard = displayBoard;
    }

    for (int x = 0; x < 13; x++)
        for (int y = 0; y < 13; y++)
            if (lastMoveHighlights[x][y])
                highlights[x][y] = true;

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

    App::onFrame();
}

void GameApp::onStop() {
    stopSource.request_stop();
    if (gameThread && gameThread->joinable())
        gameThread->join();

    delete p1;
    delete p2;
}
