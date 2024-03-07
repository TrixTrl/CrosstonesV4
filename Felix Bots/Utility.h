#pragma once

#include "../Test Bots/BasicGenerator.h"
#include "../Globals/Piece.h"
#include "GameResult.h"

#include <functional>
#include <vector>
#include <string>
#include <sstream>
#include <format>
#include <bitset>


namespace dc
{
	typedef std::vector<BasicGenerator::xMove> Move;
	typedef unsigned long long u64;

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

		static void applyXMove(uint8_t(*state)[13][13], Move& move);

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

		static bool sameMove(const Move& moveA, const Move& moveB);

		static bool sameXMove(const BasicGenerator::xMove& moveA, const BasicGenerator::xMove& moveB);

	};
}

