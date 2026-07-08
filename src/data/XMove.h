#pragma once
#include <cstdint>
#include <vector>

struct XMoveTile {
    int i;
    int j;
    uint8_t delta;
    bool operator==(const XMoveTile& o) const {
        return i == o.i && j == o.j && delta == o.delta;
    }
};

using XMove = std::vector<XMoveTile>;
