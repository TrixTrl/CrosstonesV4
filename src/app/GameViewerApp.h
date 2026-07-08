#pragma once
#include "App.h"
#include "ui/components/board.h"
#include "ui/components/dropdown.h"
#include "data/GameEntries.h"
#include "game-suite/Board.h"
#include <vector>

class GameViewerApp : public App {
protected:
    void onStart()  override;
    void onTick(float) override;
    void onDraw(Rectangle) override;
    void onDrawOverlay(Rectangle) override;

private:
    struct Entry {
        std::string name;
        std::string gameCode;
    };

    void parseAndLoadGame(const std::string& gameCode);

    Board board;
    ui::Board boardView;
    std::vector<std::vector<Board::xMove>> gameReplay;
    int currentMove = 0;
    int presetIdx = 0;
    ui::Dropdown dropdown;
    std::vector<Entry> entries;
};
