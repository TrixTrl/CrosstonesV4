#pragma once
#include <vector>
#include "Utility.h"

namespace dc
{
	class MoveOrdering
	{
	public:

		void orderMoves(std::vector<Move>& moveList, Move& bestMoveByValue);
	};

}

