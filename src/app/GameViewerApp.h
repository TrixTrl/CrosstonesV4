#pragma once
#include "App.h"
#include "game-suite/Board.h"
#include <vector>

class GameViewerApp : public App {
protected:
    std::string_view title() const override { return "Crosstones V4 - Game Viewer"; }
    int extraWidth()  const override { return 320; }

    void onStart() override;
    void onFrame() override;
    void onDrawOverlay() override;

private:
    struct GamePreset {
        const char* name;
        const char* gameCode;
    };

    void parseAndLoadGame(const std::string& gameCode);

    Board board;
    std::vector<std::vector<Board::xMove>> gameReplay;
    int currentMove = 0;
    int presetIdx = 0;
    bool dropdownOpen = false;
    int hoveredDropdownIdx = -1;
};
