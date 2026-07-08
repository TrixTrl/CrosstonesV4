#include "KsnTester.h"
#include <iostream>
#include <bitset>

KsnTester::KsnTester() {
    std::bitset<3> dragon(7);
    board.rst(dragon);
}

bool KsnTester::run() {
    bool passing = true;
    int totalRuns = 0;
    int totalPass = 0;

    auto report = [&](const char* testName, bool ok) {
        totalRuns++;
        if (ok) totalPass++;
        passing &= ok;
        std::cout << "  [" << (ok ? "PASS" : "FAIL") << "] " << testName << "\n";
        if (!ok) std::cout << "    " << error_ << "\n";
    };

    report("RoundTrip", testRoundTrip());
    report("ShortenUniqueness", testShortenUniqueness());

    if (passing)
        error_ = "All " + std::to_string(totalRuns) + " KSN tests passed";
    else
        error_ = std::to_string(totalPass) + "/" + std::to_string(totalRuns) + " KSN tests passed";
    return passing;
}

bool KsnTester::testRoundTrip() {
    auto pos = board.positionCopy();
    auto moveInfos = RichMoveGenerator::generate(pos, true);
    auto moves = KsnFormatter::enrichAll(moveInfos);
    int pass = 0;
    int failIdx = -1;
    for (size_t i = 0; i < moves.size(); i++) {
        auto rt = KsnFormatter::fromLongKsn(pos, moves[i].longKsn, true);
        if (rt == moves[i].info.xmove) {
            pass++;
        } else if (failIdx < 0) {
            failIdx = (int)i;
        }
    }
    if (pass != (int)moves.size()) {
        error_ = "RoundTrip: " + std::to_string(pass) + "/" + std::to_string(moves.size()) +
            " passed. First failure at index " + std::to_string(failIdx) +
            " KSN=" + moves[failIdx].longKsn;
    }
    return pass == (int)moves.size();
}

bool KsnTester::testShortenUniqueness() {
    auto pos = board.positionCopy();
    auto moveInfos = RichMoveGenerator::generate(pos, true);
    auto moves = KsnFormatter::enrichAll(moveInfos);
    int pass = 0;
    for (size_t i = 0; i < moves.size(); i++) {
        auto shortKsn = KsnFormatter::shorten((int)i, moves);
        auto indices = KsnFormatter::match(shortKsn, moves);
        if (indices.size() == 1 && indices[0] == (int)i)
            pass++;
    }
    if (pass != (int)moves.size()) {
        error_ = "ShortenUniqueness: " + std::to_string(pass) + "/" +
            std::to_string(moves.size()) + " unique";
    }
    return pass == (int)moves.size();
}

REGISTER_TEST(KsnTester)
