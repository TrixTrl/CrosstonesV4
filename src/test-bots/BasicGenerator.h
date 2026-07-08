#pragma once

#include "data/XMove.h"
#include <cstdint>
#include <vector>
#include <memory>

class BasicGenerator
{
public:
	using xMove = XMoveTile;
	
	static std::shared_ptr<std::vector<XMove>> getMoves(const bool isWhite, const uint8_t (*pieces)[13][13]);
	static void basicGenerator(std::shared_ptr<std::vector<XMove>> moves, uint8_t(*state)[13][13], int x, int y, bool(*visited)[13][13], int remainingSteps, bool turned, bool isWhite, const uint8_t(*pieces)[13][13]);
	static void captureGenerator(std::shared_ptr<std::vector<XMove>> moves, uint8_t(*state)[13][13], int originX, int originY, int x, int y, bool(*visited)[13][13], int remainingSteps, bool turned, bool isWhite, const uint8_t(*pieces)[13][13]);
	static const uint8_t hasTurnPiece = 128;
	static const uint8_t setTurnPiece = 64;
};

