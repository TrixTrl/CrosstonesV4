#include "Openings.h"
#include <fstream>
#include <sstream>

Openings Openings::loadAll() {
    Openings result;
    std::ifstream file("resources/openings.txt");
    if (!file.is_open()) return result;

    std::string line;
    OpeningEntry current;
    bool hasCurrent = false;

    auto flush = [&] {
        if (hasCurrent && !current.name.empty() && !current.ksnMoves.empty())
            result.entries.push_back(current);
        current = OpeningEntry{};
        hasCurrent = false;
    };

    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        if (line.substr(0, 5) == "root=") {
            result.rootPos = line.substr(5);
        } else if (line[0] == '[') {
            flush();
            auto end = line.find(']');
            current.name = line.substr(1, end == std::string::npos ? line.size() - 1 : end - 1);
            hasCurrent = true;
        } else if (line.substr(0, 5) == "code=") {
            current.code = line.substr(5);
        } else if (line.substr(0, 6) == "modes=") {
            std::stringstream ss(line.substr(6));
            int m;
            while (ss >> m) current.modes.push_back(m);
        } else if (line.substr(0, 6) == "moves=") {
            std::stringstream ss(line.substr(6));
            std::string tok;
            while (ss >> tok) current.ksnMoves.push_back(tok);
        }
    }
    flush();

    return result;
}
