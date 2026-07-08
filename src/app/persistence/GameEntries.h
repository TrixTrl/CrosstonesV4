#pragma once
#include <string>
#include <vector>

struct GameEntry {
    std::string name;
    std::string player1;
    std::string player2;
    std::string position;
    std::vector<int> moves;
};

struct GameEntries {
    std::string rootPos;
    std::vector<GameEntry> saves;

    static GameEntries loadAll();
    static void appendSave(const GameEntry& entry);
};
