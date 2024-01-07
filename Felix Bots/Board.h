#pragma once
#include <cstdint>
#include "Utility.h"

namespace dc
{ 
	class Board
	{
		// The bots representation of the current state of the game

	public:

		uint64_t zobristKey;
		uint8_t square[13][13];
		bool isWhiteTurn;

		Board() {}

		void initialize(uint8_t(*state)[13][13], bool isWhiteTurn);

		void makeMove(Move& move);
		void unmakeMove(Move& move);
	};
}
