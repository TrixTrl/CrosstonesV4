#pragma once

#include <cstdint>
#include <algorithm>
#include <vector>
#include <cstring>

struct GamePosition {
    uint8_t pieces[13][13];

    uint8_t (&operator[](size_t x))[13] {
        return pieces[x];
    }
    const uint8_t (&operator[](size_t x) const)[13] {
        return pieces[x];
    }

    bool operator==(const GamePosition& other) const {
        return std::memcmp(pieces, other.pieces, sizeof(pieces)) == 0;
    }
    bool operator<(const GamePosition& other) const {
        return std::memcmp(pieces, other.pieces, sizeof(pieces)) < 0;
    }
};