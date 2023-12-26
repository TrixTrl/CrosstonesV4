#pragma once

#include <cstdint>
#include <string>

#include "../Test Bots/BasicGenerator.h"

class Utils
{
public:
	enum class winValue {
		none,
		white,
		black,
		draw
	};

	static winValue gameOver(bool isWhite, uint8_t (*pieces)[13][13]);
	static float basicPosEval(bool isWhite, uint8_t(*pieces)[13][13]);
	static int getBestMoveBasic(bool isWhite, uint8_t(*pieces)[13][13]);
	static int trivialBestMove(bool isWhite, uint8_t(*pieces)[13][13]);

	static void print(std::string s, bool newLine);
	static void print(int s, bool newLine);
	static void print(size_t s, bool newLine);
	static void print(float s, bool newLine);
	static void print(BasicGenerator::xMove, bool newLine);

	struct debugContainer {
		int n = 0;
		int depthCounts[5] = {0, 0, 0, 0, 0};
	};

	struct xMove {
		int i;
		int j;
		uint8_t delta;
		xMove(int I, int J, uint8_t D) : i(I), j(J), delta(D) {}
	};
	static std::shared_ptr<std::vector<std::vector<xMove>>> getMoves_halfSplit_noPush(bool isWhite, uint8_t(*pieces)[13][13]);
	static void basicGenerator_halfSplit_noPush(std::shared_ptr<std::vector<std::vector<xMove>>> moves, uint8_t(*state)[13][13], int x, int y, bool(*visited)[13][13], int remainingSteps, bool turned, bool isWhite, uint8_t(*pieces)[13][13]);
	static void captureGenerator_singleSplit(std::shared_ptr<std::vector<std::vector<xMove>>> moves, uint8_t(*state)[13][13], int originX, int originY, int x, int y, bool(*visited)[13][13], int remainingSteps, bool turned, bool isWhite, uint8_t(*pieces)[13][13]);
	static const uint8_t turnPiece = 128;
	static const uint8_t setTurnPiece = 64;

	static float alphaBeta(uint8_t(*pieces)[13][13], int depth, float alpha, float beta, bool maximizing, float (*evalFunc)(bool isWhite, uint8_t(*pieces)[13][13]), debugContainer* debug);
};

