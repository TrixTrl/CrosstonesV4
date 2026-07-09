#pragma once
#include <cstdint>

struct PathStep {
    int i;
    int j;
    int splitOnwards = 0;
    bool turned = false;
    bool captured = false;
    int capturedHeight = 0;
    bool pushed = false;
    bool merged = false;
};
