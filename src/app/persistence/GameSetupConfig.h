#pragma once
#include <string>

// Persists GameApp's setup-form choices (player types/params, game mode)
// across launches, following the same key=value text file convention as
// GameEntries.
struct GameSetupConfig {
    std::string player1Type = "Deepchad";
    std::string player1Param = "4";
    std::string player2Type = "AlphaCruncher";
    std::string player2Param = "4";
    int gameModeBits = 2;

    static GameSetupConfig load();
    void save() const;
};
