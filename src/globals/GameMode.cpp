#include "GameMode.h"

namespace {

const char* const kNames[kGameModeCount] = {
	"Phoenix", "Monkey", "Lotus", "Kylin",
	"Tortoise", "Orchid", "Crane", "Dragon",
};

enum class Category { Gates, Bases, Ports };

// The 20 turning squares in order for Board::loadPosition()
constexpr Category kTurnSquareOrder[20] = {
	Category::Ports, Category::Ports, Category::Ports, Category::Ports,
	Category::Gates, Category::Ports, Category::Ports, Category::Gates,
	Category::Bases, Category::Bases, Category::Bases, Category::Bases,
	Category::Gates, Category::Ports, Category::Ports, Category::Gates,
	Category::Ports, Category::Ports, Category::Ports, Category::Ports,
};

} // namespace

const char* gameModeName(GameMode mode) {
	return kNames[static_cast<int>(mode)];
}

GameMode gameModeFromInt(int bits) {
	if (bits < 0) bits = 0;
	if (bits >= kGameModeCount) bits = kGameModeCount - 1;
	return static_cast<GameMode>(bits);
}

bool gameModeGatesVertical(GameMode mode) {
	return (static_cast<int>(mode) & 1) != 0;
}

bool gameModeBasesVertical(GameMode mode) {
	return (static_cast<int>(mode) & 2) != 0;
}

bool gameModePortsVertical(GameMode mode) {
	return (static_cast<int>(mode) & 4) != 0;
}

std::bitset<3> gameModeToTps(GameMode mode) {
	std::bitset<3> tps;
	tps[0] = gameModeGatesVertical(mode);
	tps[1] = gameModeBasesVertical(mode);
	tps[2] = gameModePortsVertical(mode);
	return tps;
}

GameMode gameModeFromTurningSquareToken(const std::string& token) {
	for (int m = 0; m < kGameModeCount; m++) {
		GameMode mode = static_cast<GameMode>(m);
		if (gameModeTurningSquareToken(mode) == token) return mode;
	}
	return GameMode::Phoenix;
}

std::string gameModeTurningSquareToken(GameMode mode) {
	std::string token(20, '0');
	for (int n = 0; n < 20; n++) {
		bool vertical = false;
		switch (kTurnSquareOrder[n]) {
			case Category::Gates: vertical = gameModeGatesVertical(mode); break;
			case Category::Bases: vertical = gameModeBasesVertical(mode); break;
			case Category::Ports: vertical = gameModePortsVertical(mode); break;
		}
		token[n] = vertical ? '1' : '0';
	}
	return token;
}
