#pragma once

#include "../Test Bots/BasicGenerator.h"
#include <functional>
#include <vector>
#include <string>

struct DCUtils
{
	enum class WinValue {
		none,
		white,
		black,
		draw
	};


	static inline void wrapWithXMove(uint8_t(*board)[13][13], std::vector<BasicGenerator::xMove>& move, std::function<void()> action);
	static void applyXMove(uint8_t(*board)[13][13], std::vector<BasicGenerator::xMove>& move);

	static WinValue calcWinValue(uint8_t(*board)[13][13], bool isWhite);
	static bool gameOver(uint8_t(*board)[13][13]);

	/* debug printing */
	static void print(std::string s, bool newLine);
	static void print(char* chars, bool newLine);
	static void print(int s, bool newLine);
	static void print(size_t s, bool newLine);
	static void print(float s, bool newLine);
	static void print(BasicGenerator::xMove, bool newLine);

};

