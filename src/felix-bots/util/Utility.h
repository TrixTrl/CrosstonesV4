#pragma once

#include "test-bots/BasicGenerator.h"
#include "globals/Piece.h"
#include "felix-bots/board/GameResult.h"

#include <functional>
#include <vector>
#include <string>
#include <sstream>
#include <format>
#include <bitset>


namespace dc
{
	/*struct xMove {
		int i;
		int j;
		uint8_t delta;
		xMove(int I, int J, uint8_t D) : i(I), j(J), delta(D) {}
	};*/

	typedef std::vector<BasicGenerator::xMove> Move;
	typedef unsigned long long u64;

	struct Utility
	{
	public:

		static const int immediateWinScore = 100000;

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

		static bool isStartingBoard(uint8_t(*state)[13][13]);
		static bool isLeftMove(Move& move);
		static Move mirrorMoveLR(Move& move);

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

