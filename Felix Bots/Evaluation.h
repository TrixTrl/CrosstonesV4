#pragma once
#include "Board.h"

namespace dc
{
	class Evaluation
	{
	public:

		static const int positionMap[13][13];
		static const int pieceValue[22];

		const int positionWeight = 8;
		const int materialWeight = 25;
		const int relativeWeight = 5; //exp

		int evaluate(const Board& board);

	};
}