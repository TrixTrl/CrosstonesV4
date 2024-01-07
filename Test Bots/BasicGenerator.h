#pragma once

#include <cstdint>
#include <vector>
#include <memory>

class BasicGenerator
{
public:
	struct xMove {
		int i;
		int j;
		uint8_t delta;
		xMove(int I, int J, uint8_t D) : i(I), j(J), delta(D) {}
	};
	static std::shared_ptr<std::vector<std::vector<xMove>>> getMoves(const bool isWhite, const uint8_t (*pieces)[13][13]);
	static void basicGenerator(std::shared_ptr<std::vector<std::vector<xMove>>> moves, uint8_t(*state)[13][13], int x, int y, bool(*visited)[13][13], int remainingSteps, bool turned, bool isWhite, const uint8_t(*pieces)[13][13]);
	static void captureGenerator(std::shared_ptr<std::vector<std::vector<xMove>>> moves, uint8_t(*state)[13][13], int originX, int originY, int x, int y, bool(*visited)[13][13], int remainingSteps, bool turned, bool isWhite, const uint8_t(*pieces)[13][13]);
	static const uint8_t turnPiece = 128;
	static const uint8_t setTurnPiece = 64;
};

