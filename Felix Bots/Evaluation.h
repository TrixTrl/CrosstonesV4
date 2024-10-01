#pragma once
#include "Board.h"

namespace dc
{
	class Evaluation
	{
	public:

		static const int positionMap[13][13];
		static const int pieceValue[22];

		const int positionWeight = 20;
		const int materialWeight = 100;
		const int weightedMatWeight = 10;
		const int relativeWeight = 100;
		const int relativeExp = 4; //exp

		int evaluate(const Board& board);
		int getEffectivePieceWorth(const Board& board, const uint8_t piece, const int x, const int y) const;
	};
}