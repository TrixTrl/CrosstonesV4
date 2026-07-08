#pragma once
#include <cstdint>

struct PathStep {
    int i;
    int j;
    int splitOnwards;
    bool turned = false;
    bool captured = false;
    int capturedHeight = 0;
    bool pushed = false;
};
