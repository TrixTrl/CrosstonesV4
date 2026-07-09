#pragma once

#include <bitset>
#include <string>

enum class GameMode : int {
	Phoenix = 0,
	Monkey = 1,
	Lotus = 2,
	Kylin = 3,
	Tortoise = 4,
	Orchid = 5,
	Crane = 6,
	Dragon = 7,
};

constexpr int kGameModeCount = 8;

const char* gameModeName(GameMode mode);

GameMode gameModeFromInt(int bits);


bool gameModeGatesVertical(GameMode mode);
bool gameModeBasesVertical(GameMode mode);
bool gameModePortsVertical(GameMode mode);

// for Board::rst() — bit0=Gates, bit1=Bases, bit2=Ports
std::bitset<3> gameModeToTps(GameMode mode);

// turning-square alignment token for to a position string (see Board::loadPosition)
std::string gameModeTurningSquareToken(GameMode mode);

// Reverse of the above
GameMode gameModeFromTurningSquareToken(const std::string& token);
