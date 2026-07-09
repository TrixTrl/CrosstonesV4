#include "MoveGenTester.h"
#include "data/GamePosition.h"
#include "data/XMove.h"
#include "data/MoveInfo.h"
#include "game-suite/Board.h"
#include "game-suite/RichMoveGenerator.h"
#include "felix-bots/move-gen/MoveGenerator.h"
#include "trix-bots/utils/FastMoveGenerator.h"
#include "trix-bots/utils/TrixBoardState.h"
#include "test-bots/BasicGenerator.h"

#include <algorithm>
#include <bitset>
#include <cstring>
#include <functional>
#include <sstream>
#include <vector>

// ── position loading helpers ────────────────────────────────────────

namespace {

void loadRaw(const std::string& posStr, uint8_t (&out)[13][13]) {
    Board b;
    b.rst(GameMode::Phoenix);
    b.loadPosition(posStr);
    b.copyPositionTo(&out);
}

GamePosition loadGamePos(const std::string& posStr) {
    uint8_t raw[13][13];
    loadRaw(posStr, raw);
    GamePosition gp;
    std::memcpy(gp.pieces, raw, sizeof(raw));
    return gp;
}

// ── generator adapters ──────────────────────────────────────────────

using GeneratorAdapter = std::function<std::vector<GamePosition>(const std::string& posStr, bool isWhite)>;

std::vector<GamePosition> genReferenceBoard(const std::string& posStr, bool isWhite) {
    Board b;
    b.rst(GameMode::Phoenix);
    b.loadPosition(posStr);
    auto moves = b.getMoves(isWhite);
    GamePosition start = b.positionCopy();

    std::vector<GamePosition> out;
    out.reserve(moves.size());
    for (auto& mv : moves) {
        GamePosition next = start;
        for (auto& t : mv) next[t.i][t.j] ^= t.delta;
        out.push_back(next);
    }
    return out;
}

std::vector<GamePosition> genRich(const std::string& posStr, bool isWhite) {
    GamePosition start = loadGamePos(posStr);
    auto infos = RichMoveGenerator::generate(start, isWhite);

    std::vector<GamePosition> out;
    out.reserve(infos.size());
    for (auto& mi : infos) {
        GamePosition next = start;
        for (auto& t : mi.xmove) next[t.i][t.j] ^= t.delta;
        out.push_back(next);
    }
    return out;
}

std::vector<GamePosition> genDc(const std::string& posStr, bool isWhite) {
    GamePosition start = loadGamePos(posStr);
    std::vector<XMove> moves;
    dc::MoveGenerator::getMovesStatic(&moves, start, isWhite);

    std::vector<GamePosition> out;
    out.reserve(moves.size());
    for (auto& mv : moves) {
        GamePosition next = start;
        for (auto& t : mv) next[t.i][t.j] ^= t.delta;
        out.push_back(next);
    }
    return out;
}

std::vector<GamePosition> genFast(const std::string& posStr, bool isWhite) {
    uint8_t raw[13][13];
    loadRaw(posStr, raw);
    auto moves = FastMoveGenerator::getMoves(&raw, isWhite);

    std::vector<GamePosition> out;
    out.reserve(moves.size());
    for (auto& mv : moves) {
        uint8_t next[13][13];
        std::memcpy(next, raw, sizeof(raw));
        FastMoveGenerator::applyMove(&next, mv);
        GamePosition gp;
        std::memcpy(gp.pieces, next, sizeof(next));
        out.push_back(gp);
    }
    return out;
}

std::vector<GamePosition> genBoardStateT(const std::string& posStr, bool isWhite) {
    BoardState_T bs;
    std::bitset<3> mode(0);
    bs.rst(mode);
    bs.loadPos(posStr);
    auto moves = bs.getMoves(isWhite, false, false);

    uint8_t raw[13][13];
    bs.copyBoard(&raw);

    std::vector<GamePosition> out;
    out.reserve(moves->size());
    for (auto& mv : *moves) {
        uint8_t next[13][13];
        std::memcpy(next, raw, sizeof(raw));
        for (auto& t : mv) next[t.i][t.j] ^= t.delta;
        GamePosition gp;
        std::memcpy(gp.pieces, next, sizeof(next));
        out.push_back(gp);
    }
    return out;
}

std::vector<GamePosition> genBasic(const std::string& posStr, bool isWhite) {
    uint8_t raw[13][13];
    loadRaw(posStr, raw);
    auto moves = BasicGenerator::getMoves(isWhite, &raw);

    GamePosition start;
    std::memcpy(start.pieces, raw, sizeof(raw));

    std::vector<GamePosition> out;
    out.reserve(moves->size());
    for (auto& mv : *moves) {
        GamePosition next = start;
        for (auto& t : mv) next[t.i][t.j] ^= t.delta;
        out.push_back(next);
    }
    return out;
}

// ── comparison core ─────────────────────────────────────────────────

struct VerifyDiagnostics {
    std::string startPositionStr;
    size_t referenceCount = 0, candidateCount = 0;
    std::vector<GamePosition> missingInCandidate; // reference has it, candidate doesn't
    std::vector<GamePosition> extraInCandidate;   // candidate has it, reference doesn't
};

bool verifyPosition(const std::string& posStr, bool isWhite,
                     const GeneratorAdapter& reference, const GeneratorAdapter& candidate,
                     VerifyDiagnostics* outDiag = nullptr) {
    auto refPositions = reference(posStr, isWhite);
    auto candPositions = candidate(posStr, isWhite);

    // Board::getMoves() always includes one explicit pass entry 
    GamePosition start = loadGamePos(posStr);
    auto isNoOp = [&](const GamePosition& p) { return p == start; };
    refPositions.erase(std::remove_if(refPositions.begin(), refPositions.end(), isNoOp), refPositions.end());
    candPositions.erase(std::remove_if(candPositions.begin(), candPositions.end(), isNoOp), candPositions.end());

    std::sort(refPositions.begin(), refPositions.end());
    std::sort(candPositions.begin(), candPositions.end());

    std::vector<GamePosition> missing, extra;
    std::set_difference(refPositions.begin(), refPositions.end(),
                         candPositions.begin(), candPositions.end(), std::back_inserter(missing));
    std::set_difference(candPositions.begin(), candPositions.end(),
                         refPositions.begin(), refPositions.end(), std::back_inserter(extra));

    if (outDiag) {
        outDiag->startPositionStr = posStr;
        outDiag->referenceCount = refPositions.size();
        outDiag->candidateCount = candPositions.size();
        size_t missingN = (std::min)(missing.size(), (size_t)5);
        size_t extraN = (std::min)(extra.size(), (size_t)5);
        outDiag->missingInCandidate.assign(missing.begin(), missing.begin() + missingN);
        outDiag->extraInCandidate.assign(extra.begin(), extra.begin() + extraN);
    }

    return missing.empty() && extra.empty();
}

// ── rule-interaction test position catalog ─────────────────────────

struct MoveGenCase {
    const char* name;
    const char* rule;
    const char* position;
    bool isWhite;
};

const std::vector<MoveGenCase>& moveGenCatalog() {
    static const std::vector<MoveGenCase> catalog = {
        // ── Movement ──
        {"Height1_FourSteps", "height-1 tower gets up to 4 steps",
         "-B10604 00000000000000000000", false},
        {"Height4_TwoSteps", "height-4/5 tower gets up to 2 steps",
         "-B40604 00000000000000000000", false},
        {"Height3_ThreeSteps", "height-2/3 tower gets up to 3 steps",
         "-W30604 00000000000000000000", true},
        {"VoidBlocksMovement", "a Void square cannot be entered",
         "-B10304 00000000000000000000", false},
        {"BoardEdgeBlocksMovement", "the board boundary cannot be crossed",
         "-W10004 00000000000000000000", true},

        // ── Turning squares ──
        {"TurnSquare_Aligned_Enter", "vertically-aligned turn square can be entered from the north",
         "-B10601 00000000000000000000", false},
        {"TurnSquare_Misaligned_Blocked", "a turn square blocks entry when misaligned",
         "-B10601 00000000100000000000", false},
        {"TurnSquare_TurnOnly_NoSteps", "turning in place with zero steps is a legal move",
         "-B10600 00000000000000000000", false},

        // ── Splitting ──
        {"Split_MidPath_Height3", "splitting at different points along a path yields distinct positions",
         "-B30604 00000000000000000000", false},
        {"Split_MidPath_Height5", "same, at max height for more split variety",
         "-B50604 00000000000000000000", false},

        // ── Merging ──
        {"Merge_OwnTower_Legal", "merging onto own tower within height-5 limit is legal",
         "-B20404 -B20604 00000000000000000000", false},
        {"Merge_ExceedsFive_Illegal", "a merge that would exceed height 5 is illegal",
         "-B30404 -B30604 00000000000000000000", false},
        {"Merge_OntoBlueNeutral", "merging onto a neutral Blue piece",
         "-B20404 b-10604 00000000000000000000", false},
        {"Merge_OntoRedNeutral", "merging onto a neutral Red piece",
         "-B20404 r-10604 00000000000000000000", false},
        {"BlueAddon_MustSplitBeforeMerge", "a tower carrying Blue cannot merge without splitting it off first",
         "bB20404 -B20604 00000000000000000000", false},

        // ── Pushing ──
        {"Push_EqualHeight_Legal", "pushing an equal-height own tower into an empty square",
         "-B20404 -B20504 00000000000000000000", false},
        {"Push_TooTall_Illegal", "pushing a taller own tower is illegal",
         "-B10404 -B30504 00000000000000000000", false},
        {"Push_Chain_Legal", "a chain push where every link is <= the tower pushing it",
         "-B30404 -B20504 -B10604 00000000000000000000", false},
        {"Push_ChainBroken_Illegal", "a chain push broken by one oversized link",
         "-B20404 -B30504 00000000000000000000", false},
        {"Push_LandingOccupied_Illegal", "a push whose landing square is occupied is illegal",
         "-B20404 -B20504 b-10604 00000000000000000000", false},

        // ── Capturing ──
        {"Capture_Height1_NoSplit", "height-1 vs height-1 capture needs no split",
         "-W10404 -B10504 00000000000000000000", true},
        {"Capture_Height1_VsTaller_Illegal", "height-1 towers can never capture a taller tower",
         "-W10404 -B20504 00000000000000000000", true},
        {"Capture_TallerMustSplit", "a taller attacker must split before jumping the target",
         "-W30404 -B20504 00000000000000000000", true},
        {"Capture_LandingOnOpponent_Illegal", "landing on a square occupied by the opponent is illegal",
         "-W30404 -B20504 -B10604 00000000000000000000", true},
        {"Capture_EndsInMergeOntoFriendly", "capture ends by merging onto a friendly tower behind the target",
         "-W30404 -B20504 -W10604 00000000000000000000", true},
        {"Capture_InsufficientSteps_Illegal", "not enough remaining steps makes the capture illegal",
         "-W40004 -B20504 00000000000000000000", true},
        {"Capture_Chain_TwoTargets", "a chain of two adjacent enemy towers captured in one move",
         "-W30404 -B10504 -B10604 00000000000000000000", true},
        {"Capture_Chain_BrokenByHeight", "a capture chain broken by one target that fails the height rule",
         "-W20404 -B10504 -B30604 00000000000000000000", true},

        // ── Blue piece ──
        {"Blue_BypassesHeightRequirement", "a Blue tower can capture regardless of height",
         "bW10404 -B50504 00000000000000000000", true},
        {"Blue_ChainCapture_CostsTwoSteps", "a Blue chain capture always costs exactly 2 steps",
         "bW40404 -B10504 -B10604 00000000000000000000", true},
        {"Blue_SplitBeforeCapture_LosesBonus", "splitting Blue off before capturing forfeits the height bypass",
         "bW20404 -B30504 00000000000000000000", true},

        // ── Red piece ──
        {"Red_ExtraStep_Height1", "a Red height-1 tower gets 5 steps instead of 4",
         "rW10604 00000000000000000000", true},
        {"Red_ExtraStep_Height3", "a Red height-2/3 tower gets 4 steps instead of 3",
         "rB30604 00000000000000000000", false},
        {"Red_ExtraStep_Height5", "a Red height-4/5 tower gets 3 steps instead of 2",
         "rW50604 00000000000000000000", true},

        // ── Stress positions ──
        {"Stress_LongMixedCaptureChain", "a long chain of decreasing-height enemy towers with a Blue attacker",
         "bW50404 -B50504 -B40604 -B30704 -B20804 -B10904 00000000000000000000", true},
        {"Stress_LongPushChain", "a long push chain of four decreasing-height own towers",
         "-B50404 -B40504 -B30604 -B20704 -B10804 00000000000000000000", false},
        {"Stress_DenseMixedCluster", "a dense, mixed-ownership/height/addon cluster",
         "-B20604 -W20704 bB10804 -W30605 r-10805 b-10606 -W10706 -B20906 00000000000000000000", false},
    };
    return catalog;
}

} // namespace

bool MoveGenTester::run() {
    struct Candidate {
        const char* name;
        GeneratorAdapter fn;
    };

    const std::vector<Candidate> candidates = {
        {"RichMoveGenerator", genRich},
        {"dc::MoveGenerator", genDc},
        {"FastMoveGenerator", genFast},
        {"BoardState_T", genBoardStateT},
        {"BasicGenerator", genBasic},
    };

    bool allPass = true;
    std::ostringstream report;
    int failCount = 0;

    for (auto& cand : candidates) {
        std::ostringstream section;
        int candFailCount = 0;

        for (auto& tc : moveGenCatalog()) {
            VerifyDiagnostics diag;
            bool ok = verifyPosition(tc.position, tc.isWhite, genReferenceBoard, cand.fn, &diag);
            if (!ok) {
                candFailCount++;
                section << "  [FAIL] " << tc.name << " (" << tc.rule << ")\n"
                        << "      position: " << diag.startPositionStr << "\n"
                        << "      reference=" << diag.referenceCount << " candidate=" << diag.candidateCount
                        << " missing=" << diag.missingInCandidate.size()
                        << " extra=" << diag.extraInCandidate.size() << "\n";
            }
        }

        if (candFailCount > 0) {
            allPass = false;
            failCount += candFailCount;
            report << "=== " << cand.name << " — " << candFailCount << " check(s) failed ===\n"
                   << section.str();
        }
    }

    if (allPass) {
        error_.clear();
    } else {
        std::ostringstream summary;
        summary << failCount << " check(s) failed:\n" << report.str();
        error_ = summary.str();
    }
    return allPass;
}

REGISTER_TEST(MoveGenTester)
