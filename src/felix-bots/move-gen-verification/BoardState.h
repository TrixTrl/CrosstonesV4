#pragma once

#include <cstdint>
#include <algorithm>
#include <vector>
#include <cstring>

struct BoardState {
    uint8_t pieces[13][13];

    bool operator==(const BoardState& other) const {
        return std::memcmp(pieces, other.pieces, sizeof(pieces)) == 0;
    }
    bool operator<(const BoardState& other) const {
        return std::memcmp(pieces, other.pieces, sizeof(pieces)) < 0;
    }
};