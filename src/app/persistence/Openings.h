#pragma once
#include <string>
#include <vector>

struct OpeningEntry {
    std::string code;                  // "K5", empty for entries without a book code
    std::string name;                  // "Korean Opening, Classical Variation"
    std::vector<int> modes;            // game-mode bits (0-7) this line applies to
    std::vector<std::string> ksnMoves; // full sequence from root, alternating W/B
};

struct Openings {
    std::string rootPos;
    std::vector<OpeningEntry> entries;
    static Openings loadAll();
};
