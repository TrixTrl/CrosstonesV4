#pragma once
#include <cstdint>
#include "data/GamePosition.h"
#include "felix-bots/util/Utility.h"
#include "felix-bots/board/GameResult.h"

namespace dc
{ 
	class BotBoard
	{
		// The bots representation of the current state of the game

	public:

		uint64_t zobristKey;
		GamePosition square;
		bool isWhiteTurn = false;
		GameResult gameResult;

		BotBoard() : gameResult(GameResult::NotStarted) {}

		void initialize(const GamePosition& state, bool isWhiteTurn);

		void updateWinValue();
		void makeMove(XMove& move);
		void unmakeMove(XMove& move);

		std::string moveToString(XMove& move);
		
		/* Testing */
		u64 bulk_perft(int depth /* >= 1 */, std::function<std::vector<XMove>(const GamePosition&, bool)> getMoves);
	
	private:

		void appyMoveReversible(XMove& move);
	};
}
