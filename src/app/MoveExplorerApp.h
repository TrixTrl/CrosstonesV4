#pragma once
#include "App.h"
#include "ui/components/board.h"
#include "ui/components/dropdown.h"
#include "game-suite/Board.h"
#include <bitset>
#include <vector>

class MoveExplorerApp : public App {
protected:
    void onStart()  override;
    void onTick(float) override;
    void onDraw(Rectangle) override;
    void onDrawOverlay(Rectangle) override;

private:
    Board board;
    ui::Board boardView;
    int presetIdx = 0;
    int moveIdx = 0;
    bool paused = false;
    int autoTimer = 0;
    ui::Dropdown dropdown;
};
