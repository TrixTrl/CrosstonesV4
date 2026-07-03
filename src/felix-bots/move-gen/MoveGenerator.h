#pragma once
#include "felix-bots/util/Utility.h"

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

		void getMoves(std::vector<dc::Move>* targetList, const uint8_t(*board)[13][13], const bool isWhite, const bool onlyClaimsAndCaptures = false);
		static void getMovesStatic(std::vector<dc::Move>* targetList, const uint8_t(*board)[13][13], const bool isWhite, const bool onlyClaimsAndCaptures = false);

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

		uint8_t currentSquares[13][13];
		uint8_t origSquares[13][13];
		bool visited[13][13];

		std::vector<dc::Move>* target;

	};
}