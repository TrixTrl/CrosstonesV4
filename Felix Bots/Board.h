#pragma once
#include <cstdint>
#include "Utility.h"
#include "GameResult.h"

namespace dc
{ 
	class Board
	{
		// The bots representation of the current state of the game

	public:

		uint64_t zobristKey;
		uint8_t square[13][13];
		bool isWhiteTurn;
		GameResult gameResult;

		Board() : gameResult(GameResult::NotStarted) {}

		void initialize(uint8_t(*state)[13][13], bool isWhiteTurn);

		void updateWinValue();
		void makeMove(Move& move);
		void unmakeMove(Move& move);

		std::string moveToString(Move& move);
		
		/* Testing */
		u64 bulk_perft(int depth);/* >= 1 */
	
	private:

		void appyMoveReversible(Move& move);
	};
}
