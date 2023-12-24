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

	static void print(std::string s);
	static void print(int s);
	static void print(float s);
	static void print(BasicGenerator::xMove);
};

