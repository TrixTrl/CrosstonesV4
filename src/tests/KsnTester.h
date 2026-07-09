#pragma once
#include "TestBase.h"
#include "game-suite/Board.h"
#include "game-suite/RichMoveGenerator.h"
#include "game-suite/KsnFormatter.h"
#include <string>

class KsnTester : public TestBase {
public:
    KsnTester();
    const char* name() const override { return "KsnTester"; }
    bool run() override;
    const char* error() const override { return error_.c_str(); }

private:
    Board board;
    std::string error_;

    bool testRoundTrip();
    bool testShortenUniqueness();
    bool testFormatting();
    bool testShortening();
};
