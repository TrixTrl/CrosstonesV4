#pragma once
#include <string>
#include <vector>

struct PositionPreset {
    std::string name;
    std::string position;
};

struct PositionPresets {
    static std::vector<PositionPreset> loadAll();
};
