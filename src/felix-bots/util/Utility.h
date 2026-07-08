#pragma once

#include "test-bots/BasicGenerator.h"
#include "globals/Piece.h"
#include "felix-bots/board/GameResult.h"
#include "data/GamePosition.h"
#include "data/XMove.h"
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

	typedef unsigned long long u64;

	struct Utility
	{
	public:

		static const int immediateWinScore = 100000;

		static XMove createNullMove() 
		{
			return XMove();
		}

		// The sign function
		template <typename T>
		static inline int sgn(T val) {
			return (T(0) < val) - (val < T(0));
		}

		static void applyXMove(GamePosition& state, XMove& move);

		static bool isWinScore(int score)
		{
			const int maxMateDepth = 1000;
			return abs(score) > immediateWinScore - maxMateDepth;
		}

		static bool isStartingBoard(const GamePosition& state);
		static bool isLeftMove(XMove& move);
		static XMove mirrorMoveLR(XMove& move);

		/* debug printing */
		static void print(std::string s, bool newLine);
		static void print(char* chars, bool newLine);
		static void print(int s, bool newLine);
		static void print(size_t s, bool newLine);
		static void print(float s, bool newLine);
		static void print(BasicGenerator::xMove, bool newLine);
	};
}

