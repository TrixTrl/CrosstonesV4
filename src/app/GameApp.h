#pragma once
#include "App.h"
#include "StackThread.h"
#include "game-suite/GamePosition.h"
#include <memory>
#include <stop_token>

class ManualPlayer;
class Player;
class GameMaster;

namespace dc { class BotBoard; }

class GameApp : public App {
protected:
    std::string_view title() const override { return "Crosstones V4"; }
    int targetFPS() const override { return 60; }

    void onStart() override;
    void onFrame() override;
    void onStop() override;

public:
    ~GameApp() override;

private:
    Player* createPlayer(const std::string& type, int param, ManualPlayer** outManual);

    std::unique_ptr<GameMaster> gameMaster;
    ManualPlayer* manualP1 = nullptr;
    ManualPlayer* manualP2 = nullptr;
    Player* p1 = nullptr;
    Player* p2 = nullptr;
    std::unique_ptr<StackThread> gameThread;

    GamePosition prevBoard;
    bool lastMoveHighlights[13][13]{};
    std::stop_source stopSource;
};
