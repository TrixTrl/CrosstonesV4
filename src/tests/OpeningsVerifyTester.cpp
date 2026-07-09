#include "OpeningsVerifyTester.h"
#include "app/persistence/Openings.h"
#include "game-suite/Board.h"
#include "game-suite/RichMoveGenerator.h"
#include "game-suite/KsnFormatter.h"
#include "globals/GameMode.h"
#include <sstream>

namespace {

// Attempts to resolve every ply of `entry` against the given mode's board.
bool tryResolveEntry(const OpeningEntry& entry, const std::string& rootPos, int mode,
                      std::string& failureMsg) {
    Board b;
    b.rst(gameModeFromInt(mode));
    std::string pos = rootPos;
    size_t space = pos.rfind(' ');
    if (space != std::string::npos)
        pos = pos.substr(0, space + 1) + gameModeTurningSquareToken(gameModeFromInt(mode));
    b.loadPosition(pos);

    for (size_t mi = 0; mi < entry.ksnMoves.size(); mi++) {
        bool isWhite = (mi % 2 == 0);
        auto allInfos = RichMoveGenerator::generate(b.positionCopy(), isWhite);
        auto allKsn = KsnFormatter::enrichAll(allInfos);
        auto indices = KsnFormatter::match(entry.ksnMoves[mi], allKsn);

        if (indices.empty()) {
            std::ostringstream msg;
            msg << "  [FAIL] " << entry.name << " (mode " << mode << "): ply " << (mi + 1)
                << " \"" << entry.ksnMoves[mi] << "\" — no matching move\n";
            failureMsg = msg.str();
            return false;
        }

        XMove mv;
        if (indices.size() == 1) {
            mv = allKsn[indices[0]].info.xmove;
        } else {
            bool foundExact = false;
            for (int idx : indices) {
                if (allKsn[idx].longKsn == entry.ksnMoves[mi]) {
                    mv = allKsn[idx].info.xmove;
                    foundExact = true;
                    break;
                }
            }
            if (!foundExact) {
                std::ostringstream msg;
                msg << "  [FAIL] " << entry.name << " (mode " << mode << "): ply " << (mi + 1)
                    << " \"" << entry.ksnMoves[mi] << "\" — ambiguous, candidates:";
                for (int idx : indices) msg << " [" << allKsn[idx].longKsn << "]";
                msg << "\n";
                failureMsg = msg.str();
                return false;
            }
        }

        b.unsafeMakeMove(mv);
    }
    return true;
}

} // namespace

bool OpeningsVerifyTester::run() {
    Openings openings = Openings::loadAll();
    std::ostringstream failures;
    int failCount = 0;
    int okCount = 0;

    if (openings.rootPos.empty()) {
        error_ = "root= line missing/empty in openings.txt";
        return false;
    }

    for (auto& entry : openings.entries) {
        std::vector<int> modesToTry = entry.modes.empty()
            ? std::vector<int>{5}
            : entry.modes;

        bool entryOk = false;
        std::string firstFailureMsg;
        for (int mode : modesToTry) {
            std::string msg;
            if (tryResolveEntry(entry, openings.rootPos, mode, msg)) {
                entryOk = true;
                break;
            }
            if (firstFailureMsg.empty()) firstFailureMsg = msg;
        }

        if (entryOk) {
            okCount++;
        } else {
            failCount++;
            if (modesToTry.size() > 1) {
                failures << "  [FAIL] " << entry.name << ": failed under all "
                         << modesToTry.size() << " applicable modes; first failure:\n"
                         << firstFailureMsg;
            } else {
                failures << firstFailureMsg;
            }
        }
    }

    if (failCount == 0) {
        error_.clear();
        return true;
    }

    std::ostringstream summary;
    summary << failCount << "/" << (failCount + okCount) << " opening entries failed to resolve:\n"
            << failures.str();
    error_ = summary.str();
    return false;
}

REGISTER_TEST(OpeningsVerifyTester)
