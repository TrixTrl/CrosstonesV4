#pragma once
#include <string>
#include <vector>

struct LoadedPosition {
    std::string position;
    std::vector<int> moveHistory;
    std::string displayName;
};
