#pragma once

#include "../Globals/Player.h"
#include "DCUtils.h"
#include "../Test Bots/BasicGenerator.h"
#include <vector>
#include <queue>

class Deepchad : public Player
{
public:
	void getMoveToPlay(uint8_t(*board)[13][13], bool isWhite, int endTime);

	float negamax(uint8_t(*board)[13][13], uint8_t depth, float alpha, float beta, int color);
	int negamaxAtRoot(uint8_t(*board)[13][13], std::shared_ptr<std::vector<std::vector<BasicGenerator::xMove>>> moves,
		uint8_t depth, float alpha, float beta, int color);
	void sortMoves(uint8_t(*board)[13][13], std::shared_ptr<std::vector<std::vector<BasicGenerator::xMove>>> moves);

	float heuristic(uint8_t(*board)[13][13], bool isWhite);

	//old
	float minimax(uint8_t(*board)[13][13], uint8_t depth, float alpha, float beta, bool isWhite);
	// bad
	std::queue<std::pair<int, int>> searchRootMultBestMoves(uint8_t(*board)[13][13], std::shared_ptr<std::vector<std::vector<BasicGenerator::xMove>>> moves,
		uint8_t depth, float alpha, float beta, int color, size_t movesAmount);
};

