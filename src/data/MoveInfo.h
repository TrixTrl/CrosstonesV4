#pragma once
#include <cstdint>
#include <vector>
#include "XMove.h"
#include "PathStep.h"

struct MoveInfo {
    int startI = -1, startJ = -1;
    int endI = -1, endJ = -1;
    int finalHeight = 0;
    int finalEndI = -1, finalEndJ = -1;
    std::vector<PathStep> path;
    XMove xmove;
    bool turned = false;
    bool merged = false;
    bool pushed = false;
};
