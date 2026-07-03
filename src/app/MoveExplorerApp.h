#pragma once
#include "App.h"
#include "game-suite/Board.h"
#include <bitset>

class MoveExplorerApp : public App {
protected:
    std::string_view title() const override { return "Crosstones V4 - Move Explorer"; }
    int extraWidth()  const override { return 320; }

    void onStart()  override;
    void onFrame()  override;
    void onDrawOverlay() override;

private:
    struct PositionPreset {
        const char* name;
        const char* position;
    };

    Board board;
    int presetIdx = 0;
    int moveIdx = 0;
    bool paused = false;
    int autoTimer = 0;
    bool dropdownOpen = false;
    int hoveredDropdownIdx = -1;
};
