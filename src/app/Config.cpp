#include "Config.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <bitset>
#include <unordered_map>

static std::vector<std::string> split(const std::string& s, char delim) {
    std::vector<std::string> result;
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        if (!item.empty()) result.push_back(item);
    }
    return result;
}

AppConfig AppConfig::fromArgs(int argc, char* argv[]) {
    AppConfig config;
    if (argc < 2) return config; // No args -> show menu

    std::string modeStr = argv[1];
    if (modeStr == "game") config.mode = AppMode::Game;
    else if (modeStr == "explore") config.mode = AppMode::Explore;
    else if (modeStr == "viewer") config.mode = AppMode::Viewer;
    else if (modeStr == "test") config.mode = AppMode::Test;
    else {
        std::cerr << "Unknown mode: " << modeStr << std::endl;
        std::cerr << "Usage: crosstones [game|explore|viewer|test] [options...]" << std::endl;
        return config;
    }

    for (int i = 2; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--p1" && i + 2 < argc) {
            config.game.player1.type = argv[++i];
            config.game.player1.param = std::stoi(argv[++i]);
        }
        else if (arg == "--p2" && i + 2 < argc) {
            config.game.player2.type = argv[++i];
            config.game.player2.param = std::stoi(argv[++i]);
        }
        else if (arg == "--mode" && i + 1 < argc) {
            config.game.gameModeBits = std::stoi(argv[++i], nullptr, 2);
        }
        else if (arg == "--position" && i + 1 < argc) {
            std::string pos;
            i++;
            while (i < argc && argv[i][0] != '-') {
                if (!pos.empty()) pos += " ";
                pos += argv[i++];
            }
            i--;
            config.explore.position = pos;
            config.game.position = pos;
        }
        else if (arg == "--time" && i + 1 < argc) {
            config.game.timeControl = std::stoi(argv[++i]);
        }
        else if (arg == "--gamecode" && i + 1 < argc) {
            std::string code;
            i++;
            while (i < argc && argv[i][0] != '-') {
                if (!code.empty()) code += " ";
                code += argv[i++];
            }
            i--;
            config.viewer.gameCode = code;
        }
        else if (arg == "--config" && i + 1 < argc) {
            config.configPath = argv[++i];
        }
    }

    return config;
}

AppConfig AppConfig::fromFile(const std::string& path) {
    AppConfig config;
    std::ifstream file(path);
    if (!file.is_open()) return config;

    std::unordered_map<std::string, std::string> kv;
    std::string line;
    while (std::getline(file, line)) {
        auto eq = line.find('=');
        if (eq != std::string::npos) {
            kv[line.substr(0, eq)] = line.substr(eq + 1);
        }
    }

    if (kv.count("mode")) {
        std::string m = kv["mode"];
        if (m == "game") config.mode = AppMode::Game;
        else if (m == "explore") config.mode = AppMode::Explore;
        else if (m == "viewer") config.mode = AppMode::Viewer;
        else if (m == "test") config.mode = AppMode::Test;
    }
    if (kv.count("p1_type")) config.game.player1.type = kv["p1_type"];
    if (kv.count("p1_param")) config.game.player1.param = std::stoi(kv["p1_param"]);
    if (kv.count("p2_type")) config.game.player2.type = kv["p2_type"];
    if (kv.count("p2_param")) config.game.player2.param = std::stoi(kv["p2_param"]);
    if (kv.count("gamemode")) config.game.gameModeBits = std::stoi(kv["gamemode"], nullptr, 2);
    if (kv.count("position")) config.game.position = kv["position"];
    if (kv.count("time_control")) config.game.timeControl = std::stoi(kv["time_control"]);
    if (kv.count("game_code")) config.viewer.gameCode = kv["game_code"];

    return config;
}

void AppConfig::toFile(const std::string& path) const {
    std::ofstream file(path);
    if (!file.is_open()) return;

    auto modeStr = [](AppMode m) {
        switch (m) {
            case AppMode::Game: return "game";
            case AppMode::Explore: return "explore";
            case AppMode::Viewer: return "viewer";
            case AppMode::Test: return "test";
            default: return "none";
        }
    };

    file << "mode=" << modeStr(mode) << "\n";
    file << "p1_type=" << game.player1.type << "\n";
    file << "p1_param=" << game.player1.param << "\n";
    file << "p2_type=" << game.player2.type << "\n";
    file << "p2_param=" << game.player2.param << "\n";
    file << "gamemode=" << std::bitset<3>(game.gameModeBits).to_string() << "\n";
    if (!game.position.empty()) file << "position=" << game.position << "\n";
    file << "time_control=" << game.timeControl << "\n";
}
