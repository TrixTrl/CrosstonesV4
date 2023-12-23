#pragma once

#include "../Globals/Player.h"
#include "../Test Bots/BasicGenerator.h"
#include <vector>

class Deepchad : public Player
{
	void getMoveToPlay(uint8_t(*board)[13][13], bool isWhite, int endTime);
	float minimax(uint8_t(*board)[13][13], uint8_t depth, bool isWhite);
	float heuristic(uint8_t(*board)[13][13], bool isWhite);
	void applyXMove(uint8_t(*board)[13][13], std::vector<BasicGenerator::xMove>& move);
};

