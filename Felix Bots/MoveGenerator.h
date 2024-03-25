#pragma once
#include "Utility.h"

#include <cstdint>
#include <vector>
#include <memory>

namespace dc
{
	class MoveGenerator
	{
	public:
		struct MovesContainer
		{
		public:
			std::vector<dc::Move> captures;
			std::vector<dc::Move> claims;
			std::vector<dc::Move> others;

			MovesContainer() : captures(), claims(), others() {}

			std::vector<dc::Move> getFullList() const
			{
				std::vector<dc::Move> out;
				out.reserve(captures.size() + claims.size() + others.size());

				out.insert(out.end(),
					std::make_move_iterator(captures.begin()),
					std::make_move_iterator(captures.end()));
				out.insert(out.end(),
					std::make_move_iterator(claims.begin()),
					std::make_move_iterator(claims.end()));
				out.insert(out.end(),
					std::make_move_iterator(others.begin()),
					std::make_move_iterator(others.end()));
				return out;
			}
		};

		const uint8_t hasTurnPiece = 128;
		const uint8_t setTurnPiece = 64;

		void getMoves(std::vector<dc::Move>* targetList, const uint8_t(*board)[13][13], const bool isWhite, const bool onlyClaimsAndCaptures = false);
		static void getMovesStatic(std::vector<dc::Move>* targetList, const uint8_t(*board)[13][13], const bool isWhite, const bool onlyClaimsAndCaptures = false);

		void basicGenerator(
			int x, int y, 
			const int startX, const int startY,
			int remainingSteps, 
			bool turned, bool isWhite, 
			bool onlyClaimsAndCaptures);
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