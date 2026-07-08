#pragma once
#include "TestBase.h"
#include "game-suite/Board.h"
#include <string>

class StateTester : public TestBase {
public:
    StateTester();
    const char* name() const override { return "StateTester"; }
    bool run() override;
    const char* error() const override { return error_.c_str(); }

private:
    bool staticLegality();
    int pieceCount();
    int redCount();
    int blueCount();
    bool checkAllMoves();

    uint8_t pieces[13][13]{};
    std::string error_;
    Board BS;
};
