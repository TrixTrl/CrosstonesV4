#pragma once
#include <string>
#include <vector>
#include <functional>
#include <cstdint>

enum class AppMode {
    None,
    Game,
    Explore,
    Viewer,
    Test
};

struct PlayerConfig {
    std::string type = "Deepchad";
    int param = 4; // depth for Deepchad, time for AlphaCruncher, etc.
};

struct GameConfig {
    PlayerConfig player1;
    PlayerConfig player2;
    int gameModeBits = 2; // 0b010 = gates default
    std::string position = ""; // empty = use default start
    int timeControl = 10000;
    int enforceTime = 0;
};

struct ExploreConfig {
    std::string position = "b-10000 r-10006 b-10012 -B30500 -B30502 -W30510 -W30512 b-10606 -B30700 -B30702 -W30710 -W30712 b-11200 r-11206 b-11212 11111111111111111111";
};

struct ViewerConfig {
    // DC(white) vs v1 of AlphaCruncher — uncomment a different game code to view other games
    std::string gameCode = "b-10000 r-10006 b-10012 -B30500 -B30502 -W30510 -W30512 b-10606 -B30700 -B30702 -W30710 -W30712 b-11200 r-11206 b-11212 11111111111111111111 | 20 95 17 151 141 29 37 125 35 175 148 6 29 102 105 163 73 108 105 250 16 127 111 179 29 312 29 103 28 163";
};

struct TestConfig {
    std::string suite = "dc"; // "dc" for now
};

struct AppConfig {
    AppMode mode = AppMode::None;
    GameConfig game;
    ExploreConfig explore;
    ViewerConfig viewer;
    TestConfig test;

    // Config file path
    std::string configPath = "crosstones_config.json";

    // Parse from command line args
    static AppConfig fromArgs(int argc, char* argv[]);
    
    // Load/save config file
    static AppConfig fromFile(const std::string& path);
    void toFile(const std::string& path) const;
};
