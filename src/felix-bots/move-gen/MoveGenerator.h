#pragma once
#include "felix-bots/util/Utility.h"
#include "game-suite/GamePosition.h"

#include <cstdint>
#include <vector>
#include <memory>

namespace dc
{
	class MoveGenerator
	{
	public:
		const uint8_t hasTurnPiece = 128;
		const uint8_t setTurnPiece = 64;

		void getMoves(std::vector<dc::Move>* targetList, const GamePosition& state, const bool isWhite, const bool onlyClaimsAndCaptures = false);
		static void getMovesStatic(std::vector<dc::Move>* targetList, const GamePosition& state, const bool isWhite, const bool onlyClaimsAndCaptures = false);

		void basicGenerator(
			int x, int y, 
			const int startX, const int startY,
			int remainingSteps, 
			bool turned, bool isWhite, 
			bool onlyClaimsAndCaptures, bool leftBehindAddon, bool leftRed);
		void captureGenerator(
			int originX, int originY, 
			int x, int y, 
			const int startX, const int startY,
			int remainingSteps, 
			bool turned, bool isWhite);
	private:
		static MoveGenerator moveGen;

		GamePosition currentSquares;
		GamePosition origSquares;
		bool visited[13][13];

		std::vector<dc::Move>* target;

	};
}