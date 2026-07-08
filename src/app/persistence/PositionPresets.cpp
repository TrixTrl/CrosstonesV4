#include "PositionPresets.h"
#include <fstream>

std::vector<PositionPreset> PositionPresets::loadAll() {
    std::vector<PositionPreset> result;
    std::ifstream file("resources/presets.txt");
    if (!file.is_open()) return result;

    std::string line, currentName, currentPos;
    auto flush = [&] {
        if (currentName.empty() || currentPos.empty()) return;
        result.push_back({currentName, currentPos});
        currentName.clear();
        currentPos.clear();
    };

    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        if (line[0] == '[') {
            flush();
            auto end = line.find(']');
            currentName = line.substr(1, end == std::string::npos ? line.size() - 1 : end - 1);
        } else if (line.substr(0, 4) == "pos=") {
            currentPos = line.substr(4);
        }
    }
    flush();

    return result;
}
