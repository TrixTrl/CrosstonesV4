#pragma once
#include <vector>
#include "data/GamePosition.h"
#include "data/MoveInfo.h"

class RichMoveGenerator {
public:
    static std::vector<MoveInfo> generate(const GamePosition& pos, bool isWhite);
};
