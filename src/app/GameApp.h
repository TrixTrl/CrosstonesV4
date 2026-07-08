#pragma once
#include "App.h"
#include "StackThread.h"
#include "ui/components/board.h"
#include "ui/components/stepper.h"
#include "ui/components/int_field.h"
#include "ui/components/dropdown.h"
#include "data/GameEntries.h"
#include "data/GamePosition.h"
#include <memory>
#include <stop_token>

class ManualPlayer;
class Player;
class GameMaster;

namespace dc { class BotBoard; }

class GameApp : public App {
public:
    struct Parameters {
        std::string player1Type = "Deepchad";
        std::string player1Param = "4";
        std::string player2Type = "AlphaCruncher";
        std::string player2Param = "4";
        int gameModeBits = 2;
    };

    enum PosSource { POS_START, POS_PRESET, POS_SAVED };

    GameApp();
    ~GameApp() override;

protected:
    void onStart() override;
    void onTick(float dt) override;
    void onDraw(Rectangle contentRect) override;
    void onDrawOverlay(Rectangle contentRect) override;
    void onStop() override;

private:
    enum Phase { SETUP, PLAYING };
    Phase phase = SETUP;

    ui::Board boardView;

    // SETUP state
    struct {
        ui::Stepper p1Stepper;
        ui::Stepper p2Stepper;
        ui::IntField p1Field;
        ui::IntField p2Field;
        ui::Dropdown mainDropdown;
        GameEntries gameEntries;
        PosSource posSource = POS_START;
        int lastGmIdx = 2;
        int presetIdx = 0;
        int saveIdx = 0;
        std::vector<std::string> playerTypes;
    } setup;

    // PLAYING state
    Parameters params;
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
    int saveFlashTimer = 0;

    void drawSetupForm(Rectangle rect);
    void startGame();
};
