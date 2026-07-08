#include "GameSetupConfig.h"
#include <fstream>
#include <unordered_map>

static const char* kPath = "game_setup.txt";

GameSetupConfig GameSetupConfig::load() {
    GameSetupConfig config;
    std::ifstream file(kPath);
    if (!file.is_open()) return config;

    std::unordered_map<std::string, std::string> kv;
    std::string line;
    while (std::getline(file, line)) {
        auto eq = line.find('=');
        if (eq != std::string::npos)
            kv[line.substr(0, eq)] = line.substr(eq + 1);
    }

    if (kv.count("p1_type"))  config.player1Type = kv["p1_type"];
    if (kv.count("p1_param")) config.player1Param = kv["p1_param"];
    if (kv.count("p2_type"))  config.player2Type = kv["p2_type"];
    if (kv.count("p2_param")) config.player2Param = kv["p2_param"];
    if (kv.count("gamemode")) {
        try { config.gameModeBits = std::stoi(kv["gamemode"]); } catch (...) {}
    }

    return config;
}

void GameSetupConfig::save() const {
    std::ofstream file(kPath, std::ios::trunc);
    if (!file.is_open()) return;

    file << "p1_type=" << player1Type << "\n";
    file << "p1_param=" << player1Param << "\n";
    file << "p2_type=" << player2Type << "\n";
    file << "p2_param=" << player2Param << "\n";
    file << "gamemode=" << gameModeBits << "\n";
}
