#include "KsnTester.h"
#include <iostream>
#include <functional>
#include <vector>

namespace {

GamePosition loadStartPos(const std::string& posStr) {
    Board b;
    b.rst(GameMode::Phoenix);
    b.loadPosition(posStr);
    return b.positionCopy();
}

// Returns the single MoveInfo matching pred, or nullptr if zero or more
// than one match (shouldnt happen in good test)
const MoveInfo* findUniqueMove(const std::vector<MoveInfo>& infos,
                                const std::function<bool(const MoveInfo&)>& pred) {
    const MoveInfo* found = nullptr;
    int count = 0;
    for (auto& info : infos) {
        if (pred(info)) { found = &info; count++; }
    }
    return count == 1 ? found : nullptr;
}

} // namespace

KsnTester::KsnTester() {
    board.rst(GameMode::Dragon);
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
    report("Formatting", testFormatting());
    report("Shortening", testShortening());

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

bool KsnTester::testFormatting() {
    struct Case {
        const char* name;
        const char* pos;
        bool isWhite;
        std::function<bool(const MoveInfo&)> pred;
        const char* expected;
    };

    std::vector<Case> cases = {
        {"SplitChain_TwoLevel",
         "-B30802 00000000000000000000", false,
         [](const MoveInfo& m) {
             return m.path.size() == 3 &&
                    m.path[0].i == 8 && m.path[0].j == 3 && m.path[0].splitOnwards == 0 &&
                    m.path[1].i == 8 && m.path[1].j == 4 && m.path[1].splitOnwards == 2 &&
                    m.path[2].i == 8 && m.path[2].j == 5 && m.path[2].splitOnwards == 1;
         },
         "I1110" "\xe2\x91\xa1" "9" "\xe2\x91\xa0" "8"},

        {"PlainMove_FileOmission",
         "-B20802 00000000000000000000", false,
         [](const MoveInfo& m) {
             return m.path.size() == 2 &&
                    m.path[0].i == 8 && m.path[0].j == 3 && m.path[0].splitOnwards == 0 &&
                    m.path[1].i == 8 && m.path[1].j == 4 && m.path[1].splitOnwards == 0;
         },
         "I11109"},

        {"Merge_NoTurn",
         "-B10706 -B10806 00000000000000000000", false,
         [](const MoveInfo& m) {
             return m.path.size() == 1 &&
                    m.path[0].i == 8 && m.path[0].j == 6 &&
                    m.path[0].merged && !m.path[0].turned;
         },
         "H7iM"},

        {"Merge_WithTurn",
         "-B10706 -B10806 00000000000000000000", false,
         [](const MoveInfo& m) {
             return m.path.size() == 1 &&
                    m.path[0].i == 8 && m.path[0].j == 6 &&
                    m.path[0].merged && m.path[0].turned;
         },
         "H7iMT"},

        {"Push_NoSpuriousDigit",
         "-B20802 -B20803 00000000000000000000", false,
         [](const MoveInfo& m) {
             return m.path.size() == 1 &&
                    m.path[0].i == 8 && m.path[0].j == 3 && m.path[0].pushed;
         },
         "I1110P"},

        {"Capture_HeightOne_NoDigit",
         "-W10802 -B10803 00000000000000000000", true,
         [](const MoveInfo& m) {
             return m.path.size() == 2 &&
                    m.path[0].captured &&
                    m.path[1].i == 8 && m.path[1].j == 4 && m.path[1].splitOnwards == 0;
         },
         "I11x9"},

        {"Capture_HeightThree_PlainDigit",
         "-W30802 -B20803 00000000000000000000", true,
         [](const MoveInfo& m) {
             return m.path.size() == 2 &&
                    m.path[0].captured &&
                    m.path[1].i == 8 && m.path[1].j == 4 && m.path[1].splitOnwards == 2;
         },
         "I11x29"},

        {"Capture_Chain_TwoTargets",
         "-W30802 -B10803 -B10804 00000000000000000000", true,
         [](const MoveInfo& m) {
             return m.path.size() == 3 &&
                    m.path[0].captured && m.path[1].captured &&
                    m.path[2].i == 8 && m.path[2].j == 5 && m.path[2].splitOnwards == 1;
         },
         "I11xx18"},
    };

    int pass = 0;
    std::string failures;
    for (auto& c : cases) {
        GamePosition start = loadStartPos(c.pos);
        auto infos = RichMoveGenerator::generate(start, c.isWhite);
        const MoveInfo* move = findUniqueMove(infos, c.pred);
        if (!move) {
            failures += std::string("  [FAIL] ") + c.name + ": no unique matching move found\n";
            continue;
        }
        std::string actual = KsnFormatter::formatLongKsn(*move);
        if (actual == c.expected) {
            pass++;
        } else {
            failures += std::string("  [FAIL] ") + c.name + ": expected \"" + c.expected +
                        "\" got \"" + actual + "\"\n";
        }
    }

    if (pass != (int)cases.size()) {
        error_ = "Formatting: " + std::to_string(pass) + "/" + std::to_string(cases.size()) +
            " passed\n" + failures;
    }
    return pass == (int)cases.size();
}

bool KsnTester::testShortening() {
    struct Case {
        const char* name;
        const char* pos;
        bool isWhite;
        std::function<bool(const MoveInfo&)> pred;
        const char* expected;
    };

    std::vector<Case> cases = {

        {"Unique_NoPrefixNeeded",
         "-B20802 00000000000000000000", false,
         [](const MoveInfo& m) {
             return m.path.size() == 2 &&
                    m.path[0].i == 8 && m.path[0].j == 3 && m.path[0].splitOnwards == 0 &&
                    m.path[1].i == 8 && m.path[1].j == 4 && m.path[1].splitOnwards == 0;
         },
         "i9"},

        {"Collision_FileOnlyPrefix_FromH",
         "-B10704 -B10904 00000000000000000000", false,
         [](const MoveInfo& m) {
             return m.startI == 7 && m.startJ == 4 &&
                    m.path.size() == 1 && m.path[0].i == 8 && m.path[0].j == 4;
         },
         "Hi9"},
        {"Collision_FileOnlyPrefix_FromJ",
         "-B10704 -B10904 00000000000000000000", false,
         [](const MoveInfo& m) {
             return m.startI == 9 && m.startJ == 4 &&
                    m.path.size() == 1 && m.path[0].i == 8 && m.path[0].j == 4;
         },
         "Ji9"},

        {"SplitChain_PreSplitFieldAlwaysShown",
         "-B30802 00000000000000000000", false,
         [](const MoveInfo& m) {
             return m.path.size() == 3 &&
                    m.path[0].i == 8 && m.path[0].j == 3 && m.path[0].splitOnwards == 0 &&
                    m.path[1].i == 8 && m.path[1].j == 4 && m.path[1].splitOnwards == 2 &&
                    m.path[2].i == 8 && m.path[2].j == 5 && m.path[2].splitOnwards == 1;
         },
         "i10" "\xe2\x91\xa1" "9" "\xe2\x91\xa0" "8"},

        {"SplitChain_NoSpuriousPrefixWithOtherTowerPresent",
         "-B30802 -B10204 00000000000000000000", false,
         [](const MoveInfo& m) {
             return m.path.size() == 3 &&
                    m.path[0].i == 8 && m.path[0].j == 3 && m.path[0].splitOnwards == 0 &&
                    m.path[1].i == 8 && m.path[1].j == 4 && m.path[1].splitOnwards == 2 &&
                    m.path[2].i == 8 && m.path[2].j == 5 && m.path[2].splitOnwards == 1;
         },
         "i10" "\xe2\x91\xa1" "9" "\xe2\x91\xa0" "8"},

        {"SplitChain_NoSpuriousPrefixWhenOtherTowerSharesOnlyAWaypoint",
         "-B30802 -B10807 00000000000010000000", false,
         [](const MoveInfo& m) {
             return m.path.size() == 3 &&
                    m.path[0].i == 8 && m.path[0].j == 3 && m.path[0].splitOnwards == 0 &&
                    m.path[1].i == 8 && m.path[1].j == 4 && m.path[1].splitOnwards == 2 &&
                    m.path[2].i == 8 && m.path[2].j == 5 && m.path[2].splitOnwards == 1;
         },
         "i10" "\xe2\x91\xa1" "9" "\xe2\x91\xa0" "8"},


        {"TurnPassthrough_DroppedByDefault_FlagFloats",
         "-B10706 -B10807 00000000000000000000", false,
         [](const MoveInfo& m) {
             return m.path.size() == 2 &&
                    m.path[0].i == 8 && m.path[0].j == 6 && m.path[0].turned &&
                    m.path[1].i == 8 && m.path[1].j == 7 && m.path[1].merged;
         },
         "T" "i6M"},

        {"SameTowerAmbiguity_ResolvedByRevealingDifferentTurnSquares_ViaC7",
         "-B10308 00001001000000000000", false,
         [](const MoveInfo& m) {
             return m.path.size() == 4 &&
                    m.path[0].i == 2 && m.path[0].j == 8 &&
                    m.path[1].i == 2 && m.path[1].j == 7 &&
                    m.path[2].i == 2 && m.path[2].j == 6 && m.path[2].turned &&
                    m.path[3].i == 3 && m.path[3].j == 6;
         },
         "c7T" "d"},
        {"SameTowerAmbiguity_ResolvedByRevealingDifferentTurnSquares_ViaE7",
         "-B10308 00001001000000000000", false,
         [](const MoveInfo& m) {
             return m.path.size() == 4 &&
                    m.path[0].i == 4 && m.path[0].j == 8 &&
                    m.path[1].i == 4 && m.path[1].j == 7 &&
                    m.path[2].i == 4 && m.path[2].j == 6 && m.path[2].turned &&
                    m.path[3].i == 3 && m.path[3].j == 6;
         },
         "e7T" "d"},

        {"Capture_TargetPriority",
         "-W30802 -B20803 00000000000000000000", true,
         [](const MoveInfo& m) {
             return m.path.size() == 2 &&
                    m.path[0].captured &&
                    m.path[1].i == 8 && m.path[1].j == 4 && m.path[1].splitOnwards == 2;
         },
         "x2i9"},

        {"DoublePush_CollapsesToSingleTrailingP",
         "-B10801 -B10802 00000000000000000000", false,
         [](const MoveInfo& m) {
             return m.path.size() == 2 &&
                    m.path[0].i == 8 && m.path[0].j == 2 && m.path[0].pushed &&
                    m.path[1].i == 8 && m.path[1].j == 3 && m.path[1].pushed;
         },
         "i10P"},

        {"DoublePush_ThenMerge_SinglePBeforeM",
         "-B10801 -B10802 00000000000000000000", false,
         [](const MoveInfo& m) {
             return m.path.size() == 3 &&
                    m.path[0].i == 8 && m.path[0].j == 2 && m.path[0].pushed &&
                    m.path[1].i == 8 && m.path[1].j == 3 && m.path[1].pushed &&
                    m.path[2].i == 8 && m.path[2].j == 4 && m.path[2].merged;
         },
         "i9PM"},

        {"SplitAtStart_LandingNotShown",
         "-B20802 00000000000000000000", false,
         [](const MoveInfo& m) {
             return m.path.size() == 2 &&
                    m.path[0].i == 8 && m.path[0].j == 3 && m.path[0].splitOnwards == 1 &&
                    m.path[1].i == 8 && m.path[1].j == 4 && m.path[1].splitOnwards == 0;
         },
         "\xe2\x91\xa0" "i9"},

        {"SplitAtStart_MultiplePassthroughsDropped",
         "-B20802 00000000000000000000", false,
         [](const MoveInfo& m) {
             return m.path.size() == 3 &&
                    m.path[0].i == 8 && m.path[0].j == 3 && m.path[0].splitOnwards == 1 &&
                    m.path[1].i == 8 && m.path[1].j == 4 && m.path[1].splitOnwards == 0 &&
                    m.path[2].i == 8 && m.path[2].j == 5 && m.path[2].splitOnwards == 0;
         },
         "\xe2\x91\xa0" "i8"},

        {"ChainedSplit_MiddleLandingDropped",
         "-B30802 00000000000000000000", false,
         [](const MoveInfo& m) {
             return m.path.size() == 3 &&
                    m.path[0].i == 8 && m.path[0].j == 3 && m.path[0].splitOnwards == 2 &&
                    m.path[1].i == 8 && m.path[1].j == 4 && m.path[1].splitOnwards == 1 &&
                    m.path[2].i == 8 && m.path[2].j == 5 && m.path[2].splitOnwards == 0;
         },
         "\xe2\x91\xa1" "i10" "\xe2\x91\xa0" "8"},
    };

    int pass = 0;
    std::string failures;
    for (auto& c : cases) {
        GamePosition start = loadStartPos(c.pos);
        auto infos = RichMoveGenerator::generate(start, c.isWhite);
        const MoveInfo* move = findUniqueMove(infos, c.pred);
        if (!move) {
            failures += std::string("  [FAIL] ") + c.name + ": no unique matching move found\n";
            continue;
        }
        auto allMoves = KsnFormatter::enrichAll(infos);
        int idx = -1;
        for (size_t i = 0; i < infos.size(); i++)
            if (&infos[i] == move) { idx = (int)i; break; }
        std::string actual = KsnFormatter::shorten(idx, allMoves);
        if (actual == c.expected) {
            pass++;
        } else {
            failures += std::string("  [FAIL] ") + c.name + ": expected \"" + c.expected +
                        "\" got \"" + actual + "\"\n";
        }
    }

    if (pass != (int)cases.size()) {
        error_ = "Shortening: " + std::to_string(pass) + "/" + std::to_string(cases.size()) +
            " passed\n" + failures;
    }
    return pass == (int)cases.size();
}

REGISTER_TEST(KsnTester)
