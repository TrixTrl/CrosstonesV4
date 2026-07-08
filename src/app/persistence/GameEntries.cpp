#include "GameEntries.h"
#include <fstream>
#include <sstream>

static void parseFile(const std::string& path, std::string& outRootPos,
                      std::vector<GameEntry>& outEntries) {
    std::ifstream file(path);
    if (!file.is_open()) return;

    std::string line, currentName, currentPlayer1, currentPlayer2, currentPos;
    std::vector<int> currentMoves;

    auto flush = [&] {
        if (currentName.empty() || currentMoves.empty()) return;
        GameEntry e;
        e.name = currentName;
        e.player1 = currentPlayer1;
        e.player2 = currentPlayer2;
        e.position = currentPos;
        e.moves = std::move(currentMoves);
        outEntries.push_back(std::move(e));
        currentName.clear();
        currentPlayer1.clear();
        currentPlayer2.clear();
        currentPos.clear();
        currentMoves.clear();
    };

    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        if (line.substr(0, 5) == "root=") {
            outRootPos = line.substr(5);
        } else if (line.substr(0, 4) == "pos=") {
            currentPos = line.substr(4);
        } else if (line.substr(0, 8) == "player1=") {
            currentPlayer1 = line.substr(8);
        } else if (line.substr(0, 8) == "player2=") {
            currentPlayer2 = line.substr(8);
        } else if (line[0] == '[') {
            flush();
            auto end = line.find(']');
            currentName = line.substr(1, end == std::string::npos ? line.size() - 1 : end - 1);
        } else if (line.substr(0, 6) == "moves=") {
            std::string ms = line.substr(6);
            std::stringstream ss(ms);
            int idx;
            while (ss >> idx) currentMoves.push_back(idx);
        }
    }
    flush();
}

GameEntries GameEntries::loadAll() {
    GameEntries result;
    parseFile("saved_games.txt", result.rootPos, result.saves);
    return result;
}

void GameEntries::appendSave(const GameEntry& entry) {
    std::ofstream file("saved_games.txt", std::ios::app);
    if (!file.is_open()) {
        // Try creating the file
        file.open("saved_games.txt", std::ios::trunc);
        if (!file.is_open()) return;
        file << "# Crosstones Saved Games\n\n";
    } else {
        file << "\n";
    }
    file << "[" << entry.name << "]\n";
    if (!entry.player1.empty())
        file << "player1=" << entry.player1 << "\n";
    if (!entry.player2.empty())
        file << "player2=" << entry.player2 << "\n";
    if (!entry.position.empty())
        file << "pos=" << entry.position << "\n";
    file << "moves=";
    for (size_t i = 0; i < entry.moves.size(); i++) {
        if (i > 0) file << " ";
        file << entry.moves[i];
    }
    file << "\n";
}
