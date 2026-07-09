#pragma once
#include <string>
#include <vector>
#include "data/GamePosition.h"
#include "data/KsnMove.h"
#include "data/MoveInfo.h"

class KsnFormatter {
public:
    static KsnMove enrich(const MoveInfo& info);
    static std::vector<KsnMove> enrichAll(const std::vector<MoveInfo>& infos);

    static std::string formatLongKsn(const MoveInfo& info);
    static XMove fromLongKsn(const GamePosition& pos, const std::string& ksn, bool isWhite);
    static std::vector<int> match(const std::string& ksn, const std::vector<KsnMove>& moves);
    static std::string shorten(int moveIdx, const std::vector<KsnMove>& moves);

    static inline char columnToLower(int col) { return (char)('a' + col); }
    static inline char columnToUpper(int col) { return (char)('A' + col); }
    static inline int rowToNumber(int j) { return 13 - j; }
    static inline int numberToRow(int n) { return 13 - n; }
};
