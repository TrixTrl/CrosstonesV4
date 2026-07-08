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

static bool fileLetterToCol(char c, int& col) {
    if (c >= 'a' && c <= 'm') { col = c - 'a'; return true; }
    if (c >= 'A' && c <= 'M') { col = c - 'A'; return true; }
    return false;
}

static bool parseRank(const std::string& s, size_t& pos, int& rank) {
    if (pos >= s.size() || s[pos] < '1' || s[pos] > '9') return false;
    rank = 0;
    while (pos < s.size() && s[pos] >= '0' && s[pos] <= '9') {
        rank = rank * 10 + (s[pos] - '0');
        pos++;
    }
    return rank >= 1 && rank <= 13;
}

static std::string extractSplitSuffix(const std::string& ksn) {
    std::string suffix;
    size_t p = 0;
    while (p < ksn.size()) {
        int v = parseCircled(ksn, p);
        if (v > 0) {
            suffix += circled(v);
        } else {
            p++;
        }
    }
    return suffix;
}

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

    for (size_t s = 0; s < info.path.size(); s++) {
        const auto& step = info.path[s];

        if (step.captured) {
            ksn += "x";
            if (step.capturedHeight > 0)
                ksn += std::to_string(step.capturedHeight);
        }

        ksn += columnToLower(step.i);
        ksn += std::to_string(rowToNumber(step.j));

        if (step.turned) ksn += "T";
        if (step.pushed) ksn += "P";

        if (step.splitOnwards > 0)
            ksn += circled(step.splitOnwards);
    }

    return ksn;
}

// ── xMoveToLongKsn ───────────────────────────────────────────────────

std::string KsnFormatter::xMoveToLongKsn(const GamePosition& before,
                                           const XMove& move)
{
    GamePosition after = before;
    for (auto& xm : move)
        after[xm.i][xm.j] ^= xm.delta;

    int startI = -1, startJ = -1;
    int endI = -1, endJ = -1;
    int captures[13][13] = {0};
    bool turnHappened = false;

    for (int i = 0; i < 13; i++) {
        for (int j = 0; j < 13; j++) {
            uint8_t oldV = before[i][j];
            uint8_t newV = after[i][j];
            if (oldV == newV) continue;

            int oldH = Piece::height(oldV);
            int newH = Piece::height(newV);

            if (oldH > 0 && newH < oldH && Piece::isTower(oldV))
                startI = i, startJ = j;
            if (newH > 0 && newH > oldH && Piece::isTower(newV))
                endI = i, endJ = j;
            if (oldH > 0 && newH == 0 && Piece::isTower(oldV))
                captures[i][j] = oldH;
            if ((oldV ^ newV) & 64)
                turnHappened = true;
        }
    }

    if (startI < 0 || endI < 0) return "Pass";

    MoveInfo info;
    info.startI = startI;
    info.startJ = startJ;
    info.endI = endI;
    info.endJ = endJ;
    info.finalEndI = endI;
    info.finalEndJ = endJ;
    info.finalHeight = Piece::height(after[endI][endJ]);

    info.path.push_back({startI, startJ, 0});
    if (std::abs(startI - endI) + std::abs(startJ - endJ) == 1) {
        info.path.push_back({endI, endJ, 0});
    } else {
        info.path.push_back({endI, endJ, 0});
    }

    info.turned = turnHappened;
    return formatLongKsn(info);
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
    std::vector<int> result;
    std::string s = ksnStr;

    for (int mi = 0; mi < (int)moves.size(); mi++) {
        if (moves[mi].longKsn == s)
            return {mi};
    }

    for (int mi = 0; mi < (int)moves.size(); mi++) {
        const auto& longKsn = moves[mi].longKsn;

        std::string longEnd;
        size_t lastLow = longKsn.find_last_of("abcdefghijklm");
        if (lastLow != std::string::npos) {
            size_t endStart = lastLow;
            size_t endEnd = endStart;
            while (endEnd < longKsn.size() &&
                   (std::isalpha((unsigned char)longKsn[endEnd]) ||
                    std::isdigit((unsigned char)longKsn[endEnd])))
                endEnd++;
            longEnd = longKsn.substr(endStart, endEnd - endStart);
        }

        std::string queryEnd;
        size_t qLastLow = s.find_last_of("abcdefghijklm");
        if (qLastLow != std::string::npos) {
            size_t qEnd = qLastLow;
            while (qEnd < s.size() &&
                   (std::isalpha((unsigned char)s[qEnd]) ||
                    std::isdigit((unsigned char)s[qEnd])))
                qEnd++;
            queryEnd = s.substr(qLastLow, qEnd - qLastLow);
        }

        if (!queryEnd.empty() && longEnd != queryEnd)
            continue;

        bool startFileCheck = false;
        int startFileVal = -1;
        for (size_t p = 0; p < s.size(); p++) {
            if (s[p] >= 'A' && s[p] <= 'M') {
                int col;
                if (fileLetterToCol(s[p], col)) {
                    startFileCheck = true;
                    startFileVal = col;
                }
            }
        }
        if (startFileCheck && moves[mi].info.startI != startFileVal)
            continue;

        bool startRankCheck = false;
        int startRankVal = -1;
        for (size_t p = 0; p + 1 < s.size(); p++) {
            if (s[p] >= '1' && s[p] <= '9' && s[p+1] >= 'a' && s[p+1] <= 'm') {
                startRankCheck = true;
                startRankVal = s[p] - '0';
                break;
            }
        }
        if (startRankCheck && rowToNumber(moves[mi].info.startJ) != startRankVal)
            continue;

        bool queryHasTurn = (s.find('T') != std::string::npos);
        if (queryHasTurn && !moves[mi].info.turned)
            continue;

        bool queryHasPush = (s.find('P') != std::string::npos);
        if (queryHasPush && !moves[mi].info.pushed)
            continue;

        {
            int qSteps = 0, mSteps = 0;
            for (char c : s)
                if (c >= 'a' && c <= 'm') qSteps++;
            for (char c : longKsn)
                if (c >= 'a' && c <= 'm') mSteps++;
            if (qSteps > mSteps)
                continue;
        }

        result.push_back(mi);
    }

    return result;
}

// ── shorten ──────────────────────────────────────────────────────────

std::string KsnFormatter::shorten(int moveIdx, const std::vector<KsnMove>& moves)
{
    if (moveIdx < 0 || moveIdx >= (int)moves.size())
        return "";
    if (moves.size() <= 1)
        return moves[moveIdx].longKsn;

    const auto& base = moves[moveIdx];

    int ei = base.info.finalEndI;
    int ej = base.info.finalEndJ;
    std::string endStr;
    endStr += columnToLower(ei);
    endStr += std::to_string(rowToNumber(ej));

    std::string splitSuffix = extractSplitSuffix(base.longKsn);

    for (int includeSplits = 0; includeSplits <= 1; includeSplits++) {
        std::string baseEnd = endStr;
        if (includeSplits) baseEnd += splitSuffix;

        {
            auto indices = match(baseEnd, moves);
            if ((int)indices.size() == 1 && indices[0] == moveIdx)
                return baseEnd;
        }

        std::string prefixes[] = {
            std::string(1, columnToUpper(base.info.startI)),
            std::to_string(rowToNumber(base.info.startJ)),
            std::string(1, columnToUpper(base.info.startI)) +
                std::to_string(rowToNumber(base.info.startJ))
        };

        for (auto& prefix : prefixes) {
            std::string candidate = prefix + baseEnd;
            auto indices = match(candidate, moves);
            if ((int)indices.size() == 1 && indices[0] == moveIdx)
                return candidate;
        }
    }

    {
        std::string longKsn = base.longKsn;
        size_t pos = 0;
        while (pos < longKsn.size() && longKsn[pos] >= 'A' && longKsn[pos] <= 'M')
            pos++;
        while (pos < longKsn.size() && longKsn[pos] >= '0' && longKsn[pos] <= '9')
            pos++;
        std::string stepPortion = longKsn.substr(pos);

        {
            auto indices = match(stepPortion, moves);
            if ((int)indices.size() == 1 && indices[0] == moveIdx)
                return stepPortion;
        }

        std::string prefixes[] = {
            std::string(1, columnToUpper(base.info.startI)),
            std::to_string(rowToNumber(base.info.startJ)),
            std::string(1, columnToUpper(base.info.startI)) +
                std::to_string(rowToNumber(base.info.startJ))
        };
        for (auto& prefix : prefixes) {
            std::string candidate = prefix + stepPortion;
            auto indices = match(candidate, moves);
            if ((int)indices.size() == 1 && indices[0] == moveIdx)
                return candidate;
        }
    }

    return base.longKsn;
}
