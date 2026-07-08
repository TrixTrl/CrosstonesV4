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
    void onTick(float, const InputState&) override;
    void onDraw(Rectangle) override;
    void onDrawOverlay(Rectangle) override;

private:
    struct State {
        int presetIdx = 0;
        int moveIdx = 0;
        bool paused = false;
        int autoTimer = 0;
    };
    State state;

    Board board;
    ui::Board boardView;
    ui::Dropdown dropdown;
};
