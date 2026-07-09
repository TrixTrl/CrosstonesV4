#include "KsnFormatter.h"
#include "RichMoveGenerator.h"
#include "globals/Piece.h"
#include <algorithm>
#include <set>
#include <sstream>
#include <cstring>

// ── helpers ──────────────────────────────────────────────────────────

static std::string circled(int n) {
    switch (n) {
        case 1: return "\xe2\x91\xa0";
        case 2: return "\xe2\x91\xa1";
        case 3: return "\xe2\x91\xa2";
        case 4: return "\xe2\x91\xa3";
        default: return "-" + std::to_string(n);
    }
}

static int parseCircled(const std::string& s, size_t& pos) {
    if (pos >= s.size()) return 0;
    unsigned char c = (unsigned char)s[pos];
    if (c == 0xE2 && pos + 2 < s.size()) {
        unsigned char c2 = (unsigned char)s[pos+1];
        unsigned char c3 = (unsigned char)s[pos+2];
        if (c2 == 0x91 && c3 >= 0xA0 && c3 <= 0xA3) {
            pos += 3;
            return c3 - 0xA0 + 1;
        }
    }
    if (c == '-' && pos + 1 < s.size()) {
        char d = s[pos+1];
        if (d >= '1' && d <= '4') {
            pos += 2;
            return d - '0';
        }
    }
    return 0;
}


struct SplitConstraint {
    bool fromIsStart = false;
    int fromI = -1, fromJ = -1;
    int value = 0;
};

struct ParsedKsn {
    int startFile = -1, startRank = -1;
    int finalI = -1, finalJ = -1;      
    bool hasCapture = false;
    bool hasTurn = false, hasMerge = false, hasPush = false;
    bool endsWithTurn = false;
    std::vector<SplitConstraint> splitConstraints;
};

// Deals with consecutive digits in a rank, which can only be 10-13, 
// and returns the parsed rank while advancing the position index.
static int parseRankDigits(const std::string& s, size_t& p) {
    int first = s[p] - '0';
    p++;
    if (first == 1 && p < s.size() && s[p] >= '0' && s[p] <= '3') {
        int rank = 10 + (s[p] - '0');
        p++;
        return rank;
    }
    return first;
}

// Parses an externally-authored KSN string by tracking a running position
static ParsedKsn parseKsnQuery(const std::string& s) {
    ParsedKsn r;
    if (s == "Pass") return r;

    size_t p = 0;
    if (p < s.size() && s[p] >= 'A' && s[p] <= 'M') {
        r.startFile = s[p] - 'A';
        p++;
        if (p < s.size() && s[p] >= '0' && s[p] <= '9') {
            r.startRank = parseRankDigits(s, p);
        }
    }

    int curFile = -1, curRank = -1;
    bool pendingCapture = false;

    while (p < s.size()) {
        unsigned char c = (unsigned char)s[p];

        if (c == 'x') {
            r.hasCapture = true;
            pendingCapture = true;
            p++;
            continue;
        }
        if (c == 'T') {
            r.hasTurn = true;
            p++;
            r.endsWithTurn = (p == s.size());
            continue;
        }
        if (c == 'M') { r.hasMerge = true; p++; continue; }
        if (c == 'P') { r.hasPush = true; p++; continue; }

        if (c == 0xE2) {
            size_t before = p;
            int v = parseCircled(s, p);
            if (v <= 0) { p = before + 1; continue; } // malformed, skip a byte
            bool isStart = (curFile < 0 && curRank < 0);
            r.splitConstraints.push_back({isStart, curFile, curRank >= 0 ? 13 - curRank : -1, v});
            continue;
        }
        if (c == '-' && p + 1 < s.size() && s[p + 1] >= '1' && s[p + 1] <= '4') {
            bool isStart = (curFile < 0 && curRank < 0);
            r.splitConstraints.push_back({isStart, curFile, curRank >= 0 ? 13 - curRank : -1, s[p + 1] - '0'});
            p += 2;
            continue;
        }

        if (c >= 'a' && c <= 'm') {
            curFile = c - 'a';
            // hitting a file means capture info is finished
            pendingCapture = false;
            p++;
            continue;
        }
        if (c >= '0' && c <= '9') {
            if (pendingCapture) {
                // Post-capture split count is always a single digit
                // the rest of any digit run that follows is the rank.
                r.splitConstraints.push_back({false, -1, -1, c - '0'});
                p++;
                pendingCapture = false;
                continue;
            }
            curRank = parseRankDigits(s, p);
            continue;
        }
        p++; // unrecognized byte, skip defensively
    }

    r.finalI = curFile;
    if (curRank >= 0) r.finalJ = 13 - curRank;
    return r;
}

// ── shortened-form skeleton (shared by match() and shorten()) ──────────

namespace {

// `revealWeakOps`: turning and pushing are operations too, but weaker
// priority than a split/capture departure *and* weaker than a start-square
// disambiguator, a turn/push-only passthrough square is dropped by
// default, and only forced back in as a last-resort disambiguator (when
// even the fully-qualified start square doesn't resolve an ambiguity,
// which happens when the colliding move is a different move of the
// same tower, the start square is then identical either way).
std::vector<bool> computeShownSteps(const std::vector<PathStep>& path, bool revealWeakOps) {
    std::vector<bool> shown(path.size(), false);
    if (path.empty()) return shown;

    shown[path.size() - 1] = true; // final landing square, always

    if (revealWeakOps) {
        for (size_t i = 0; i < path.size(); i++)
            if (path[i].turned || path[i].pushed)
                shown[i] = true;
    }

    for (size_t i = 0; i + 1 < path.size(); i++) {
        if (path[i + 1].splitOnwards > 0 && !path[i + 1].captured)
            shown[i] = true;
    }
    return shown;
}

std::string buildSkeleton(const MoveInfo& info, bool revealWeakOps = false) {
    if (info.path.empty()) return "Pass";

    auto shown = computeShownSteps(info.path, revealWeakOps);

    // Only print push once before the final step's own T/M/MT/simply at the end.
    bool anyPushed = false;
    for (auto& step : info.path)
        if (step.pushed) anyPushed = true;

    std::string s;
    int prevI = 0, prevJ = 0;
    bool firstCoordDone = false;
    bool pendingCapture = false;

    for (size_t idx = 0; idx < info.path.size(); idx++) {
        const auto& step = info.path[idx];

        if (step.captured) {
            s += "x";
            pendingCapture = true;
            continue;
        }

        // The split digit is always shown
        if (step.splitOnwards > 0)
            s += pendingCapture ? std::to_string(step.splitOnwards)
                                 : circled(step.splitOnwards);

        if (shown[idx]) {
            if (!firstCoordDone) {
                s += KsnFormatter::columnToLower(step.i);
                s += std::to_string(KsnFormatter::rowToNumber(step.j));
                firstCoordDone = true;
            } else {
                if (step.i != prevI) s += KsnFormatter::columnToLower(step.i);
                if (step.j != prevJ) s += std::to_string(KsnFormatter::rowToNumber(step.j));
            }
            prevI = step.i;
            prevJ = step.j;
        }

        // T/M flags are never dropped, even when the square carrying them is.
        if (idx + 1 == info.path.size() && anyPushed)
            s += "P";
        if (step.turned && step.merged) s += "MT";
        else if (step.turned) s += "T";
        else if (step.merged) s += "M";

        pendingCapture = false;
    }
    return s;
}

// Tries the skeleton with no prefix, then a file-only prefix, then the full
// start square — returns "" if none of the three are unique among moves.
std::string tryDisambiguate(const KsnMove& base, const std::vector<KsnMove>& moves, bool revealWeakOps) {
    std::string baseSkeleton = buildSkeleton(base.info, revealWeakOps);

    {
        int count = 0;
        for (auto& m : moves)
            if (buildSkeleton(m.info, revealWeakOps) == baseSkeleton) count++;
        if (count == 1) return baseSkeleton;
    }

    if (base.info.path.empty()) return "";

    char baseFile = KsnFormatter::columnToUpper(base.info.startI);
    int baseRank = KsnFormatter::rowToNumber(base.info.startJ);

    {
        std::string candidate = std::string(1, baseFile) + baseSkeleton;
        int count = 0;
        for (auto& m : moves) {
            std::string theirs = std::string(1, KsnFormatter::columnToUpper(m.info.startI)) +
                                  buildSkeleton(m.info, revealWeakOps);
            if (theirs == candidate) count++;
        }
        if (count == 1) return candidate;
    }

    {
        std::string candidate = std::string(1, baseFile) + std::to_string(baseRank) + baseSkeleton;
        int count = 0;
        for (auto& m : moves) {
            std::string theirs = std::string(1, KsnFormatter::columnToUpper(m.info.startI)) +
                                  std::to_string(KsnFormatter::rowToNumber(m.info.startJ)) +
                                  buildSkeleton(m.info, revealWeakOps);
            if (theirs == candidate) count++;
        }
        if (count == 1) return candidate;
    }

    return "";
}

} // namespace

// ── enrich ───────────────────────────────────────────────────────────

KsnMove KsnFormatter::enrich(const MoveInfo& info) {
    KsnMove km;
    km.info = info;
    km.longKsn = formatLongKsn(info);
    return km;
}

std::vector<KsnMove> KsnFormatter::enrichAll(const std::vector<MoveInfo>& infos) {
    std::vector<KsnMove> result;
    result.reserve(infos.size());
    for (auto& info : infos)
        result.push_back(enrich(info));
    return result;
}

// ── formatLongKsn ────────────────────────────────────────────────────

std::string KsnFormatter::formatLongKsn(const MoveInfo& info) {
    if (info.path.empty()) return "Pass";

    std::string ksn;
    ksn += columnToUpper(info.startI);
    ksn += std::to_string(rowToNumber(info.startJ));

    int prevI = info.startI, prevJ = info.startJ;
    bool pendingCapture = false;

    // Push carries no location of its own worth notating
    // it is always notated at the end of the move (before T/M/MT ia)
    bool anyPushed = false;
    for (auto& step : info.path)
        if (step.pushed) anyPushed = true;

    for (size_t s = 0; s < info.path.size(); s++) {
        const auto& step = info.path[s];

        // A captured square is always omitted, instead we use an 
        // "x" marker, paired with a split (eg "x3m7") 
        if (step.captured) {
            ksn += "x";
            pendingCapture = true;
            continue;
        }

        if (step.splitOnwards > 0) {
            // Capture-split counts are plain digits, normal split counts are circled digits.
            ksn += pendingCapture ? std::to_string(step.splitOnwards)
                                   : circled(step.splitOnwards);
        }

        // Doubled coordinates compared to the previous printed square may be omitted.
        if (step.i != prevI) ksn += columnToLower(step.i);
        if (step.j != prevJ) ksn += std::to_string(rowToNumber(step.j));
        prevI = step.i;
        prevJ = step.j;

        if (s + 1 == info.path.size() && anyPushed)
            ksn += "P";
        if (step.turned && step.merged) ksn += "MT";
        else if (step.turned) ksn += "T";
        else if (step.merged) ksn += "M";

        pendingCapture = false;
    }

    return ksn;
}


// ── fromLongKsn ──────────────────────────────────────────────────────

XMove KsnFormatter::fromLongKsn(const GamePosition& pos,
                                 const std::string& ksnStr,
                                 bool isWhite)
{
    auto allMoveInfos = RichMoveGenerator::generate(pos, isWhite);
    auto allKsnMoves = enrichAll(allMoveInfos);
    auto indices = match(ksnStr, allKsnMoves);
    if (indices.size() == 1)
        return allKsnMoves[indices[0]].info.xmove;
    if (indices.size() > 1) {
        for (int idx : indices) {
            if (allKsnMoves[idx].longKsn == ksnStr)
                return allKsnMoves[idx].info.xmove;
        }
    }
    return {};
}

// ── match ────────────────────────────────────────────────────────────

std::vector<int> KsnFormatter::match(const std::string& ksnStr,
                                      const std::vector<KsnMove>& moves)
{
    std::string s = ksnStr;

    for (int mi = 0; mi < (int)moves.size(); mi++) {
        if (moves[mi].longKsn == s)
            return {mi};
    }

    // Fast path: exact equality against the house-style shortened skeleton
    {
        std::vector<int> exact;
        for (int mi = 0; mi < (int)moves.size(); mi++) {
            bool found = false;
            for (bool weak : {false, true}) {
                std::string skel = buildSkeleton(moves[mi].info, weak);
                std::string withFile = std::string(1, columnToUpper(moves[mi].info.startI)) + skel;
                std::string withFull = std::string(1, columnToUpper(moves[mi].info.startI)) +
                                        std::to_string(rowToNumber(moves[mi].info.startJ)) + skel;
                if (s == skel || s == withFile || s == withFull) { found = true; break; }
            }
            if (found) exact.push_back(mi);
        }
        if (exact.size() == 1)
            return exact;
    }

    // Primary path: parse query into a resolved end square, then compare
    // against each candidate's already-known MoveInfo::finalEndI/J

    ParsedKsn q = parseKsnQuery(s);

    // T/M/P are normally optional annotations (not forced when the move
    // is unambiguous without them).
    // But, when two real candidate lines differ only in one of these flags 
    // (eg "g7" and "Tg7" are both possible), the query's stated flags 
    // (or their absence) become the only signal that tells them apart. 

    // So: try an exact match on turned/merged/pushed first.
    // Fall back to the lenient optional annotations.
    auto passesCommonFilters = [&](const MoveInfo& info) {
        if (q.finalI >= 0 || q.finalJ >= 0) {
            if (info.path.empty()) return false; // "Pass" has no target square
            if (q.finalI >= 0 && info.finalEndI != q.finalI) return false;
            if (q.finalJ >= 0 && info.finalEndJ != q.finalJ) return false;
        }

        if (q.startFile >= 0 && info.startI != q.startFile) return false;
        if (q.startRank >= 0 && rowToNumber(info.startJ) != q.startRank) return false;

        // A turn at the final square is always shown !
        if (!info.path.empty()) {
            if (info.path.back().turned != q.endsWithTurn) return false;
        }

        if (q.hasCapture) {
            bool candidateCaptures = false;
            for (auto& step : info.path)
                if (step.captured) candidateCaptures = true;
            if (!candidateCaptures) return false;
        }

        // Each split marker in the query corresponds to a real split point,
        // attached to the square it's leaving.
        // Squares not marker-attached nor the final target might be omitted from the query, 
        // so only the (fromSquare, value) pairs are reliable constraints.
        // First match the split markers themselves.
        int candidateSplitCount = 0;
        bool allConstraintsSatisfied = true;
        for (auto& constraint : q.splitConstraints) {
            bool satisfied = false;
            int scanPrevI = info.startI, scanPrevJ = info.startJ;
            for (auto& step : info.path) {
                if (step.captured) continue;
                bool fromMatches = constraint.fromIsStart
                    ? (scanPrevI == info.startI && scanPrevJ == info.startJ)
                    : (constraint.fromI < 0 || constraint.fromI == scanPrevI) &&
                      (constraint.fromJ < 0 || constraint.fromJ == scanPrevJ);
                if (fromMatches && step.splitOnwards == constraint.value) satisfied = true;
                scanPrevI = step.i;
                scanPrevJ = step.j;
            }
            if (!satisfied) { allConstraintsSatisfied = false; break; }
        }
        for (auto& step : info.path) {
            if (!step.captured && step.splitOnwards > 0) candidateSplitCount++;
        }
        if (!allConstraintsSatisfied) return false;
        if (candidateSplitCount != (int)q.splitConstraints.size()) return false;

        return true;
    };

    for (bool strict : {true, false}) {
        std::vector<int> result;
        for (int mi = 0; mi < (int)moves.size(); mi++) {
            const auto& info = moves[mi].info;
            if (!passesCommonFilters(info)) continue;

            if (strict) {
                if (q.hasTurn != info.turned) continue;
                if (q.hasMerge != info.merged) continue;
                if (q.hasPush != info.pushed) continue;
            } else {
                if (q.hasTurn && !info.turned) continue;
                if (q.hasMerge && !info.merged) continue;
                if (q.hasPush && !info.pushed) continue;
            }

            result.push_back(mi);
        }
        if (!result.empty()) return result;
    }
    return {};
}

// ── shorten ──────────────────────────────────────────────────────────

/* 
    House Style KSN Shortening Rules
    (not entirely minimal, but consistent):

 - A captured square never gets its own coordinate — only its "x".
 - Every split-occurrence square (a partial-split departure, whether
   mid-path or the post-capture landing decision) is always shown.
 - The square immediately preceding a *plain* split-occurrence square
   (a passthrough hop with no split/capture of its own) is shown too,
   since it's part of that split's identity — but only that one hop,
   not the whole preceding run. Captures are the exception: the hop
   leading into a capture is never shown; captures prefer to fall back
   on a start-square disambiguator instead of revealing that hop.
 - Every other passthrough hop is dropped.
 - The final landing square is always shown, whatever kind of step it is.
 - T/M/P flags are never dropped.
 - Among all shown coordinate tokens, the very first one is always given
   in full (never abbreviated to a bare rank) — a lone leading digit
   would be confusable with a start-square rank-only disambiguator, which
   thus avoided. Every token after the first follows a
   "doubled coordinate may be omitted" rule against the previously shown 
   token.
 - A start-square disambiguator (file only, else file+rank — never rank
   only) is prefixed only if the move isn't already unique without it.
 - A turn/push-only passthrough square has even weaker priority than the
   start square: it's dropped by default and only forced back in (still
   behind the same three prefix tiers) as a last resort, when the
   colliding move belongs to the *same* tower — the start square is then
   identical for both moves and can't disambiguate anything by itself.
 */
std::string KsnFormatter::shorten(int moveIdx, const std::vector<KsnMove>& moves)
{
    if (moveIdx < 0 || moveIdx >= (int)moves.size())
        return "";

    const auto& base = moves[moveIdx];

    std::string result = tryDisambiguate(base, moves, false);
    if (!result.empty()) return result;

    result = tryDisambiguate(base, moves, true);
    if (!result.empty()) return result;

    if (base.info.path.empty()) return "Pass";

    return base.longKsn;
}
