#pragma once
#include <cstdint>
#include "Utility.h"
#include "GameResult.h"

namespace dc
{ 
	class BotBoard
	{
		// The bots representation of the current state of the game

	public:

		uint64_t zobristKey;
		uint8_t square[13][13];
		bool isWhiteTurn = false;
		GameResult gameResult;

		BotBoard() : gameResult(GameResult::NotStarted) {}

		void initialize(uint8_t(*state)[13][13], bool isWhiteTurn);

		void updateWinValue();
		void makeMove(Move& move);
		void unmakeMove(Move& move);

		std::string moveToString(Move& move);
		
		/* Testing */
		u64 bulk_perft(int depth /* >= 1 */, std::function<std::vector<Move>(uint8_t(*)[13][13], bool)> getMoves);
	
	private:

		void appyMoveReversible(Move& move);
	};
}
