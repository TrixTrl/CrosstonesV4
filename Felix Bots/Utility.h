#pragma once

#include "../Test Bots/BasicGenerator.h"
#include "GameResult.h"
#include <functional>
#include <vector>
#include <string>
#include <sstream>
#include <format>



namespace dc
{
	typedef std::vector<BasicGenerator::xMove> Move;

	static struct Utility
	{
	public:

		static const int immediateWinScore = 100000;

		static Move nullMove;

		static class _init
		{
		public:
			_init() { nullMove = std::vector<BasicGenerator::xMove>(); }
		} _initializer;

		static Move createNullMove() 
		{
			return std::vector<BasicGenerator::xMove>();
		}

		// The sign function
		template <typename T>
		static inline int sgn(T val) {
			return (T(0) < val) - (val < T(0));
		}


		static inline void wrapWithXMove(uint8_t(*board)[13][13], Move& move,
			std::function<void()> action)
		{
			applyXMove(board, move);
			action();
			applyXMove(board, move);
		}

		static void applyXMove(uint8_t(*board)[13][13], Move& move);

		static GameResult calcWinValue(uint8_t(*board)[13][13], bool isWhite);
		static bool gameOver(uint8_t(*board)[13][13]);

		static bool isWinScore(int score)
		{
			const int maxMateDepth = 1000;
			return abs(score) > immediateWinScore - maxMateDepth;
		}

		/* debug printing */
		static void print(std::string s, bool newLine);
		static void print(char* chars, bool newLine);
		static void print(int s, bool newLine);
		static void print(size_t s, bool newLine);
		static void print(float s, bool newLine);
		static void print(BasicGenerator::xMove, bool newLine);

		static std::string toString(Move& move) 
		{
			std::string result = "";
			for (auto& xMove : move)
			{
				result += std::format("\n{} | {} | {}", xMove.i, xMove.j, xMove.delta);
			}
			return result.substr(1);
		}
	};
}

