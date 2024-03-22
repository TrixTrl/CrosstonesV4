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

		std::unique_ptr<std::vector<dc::Move>> getMoves(const bool isWhite, const uint8_t(*pieces)[13][13]);
		static std::unique_ptr<std::vector<dc::Move>> getMovesStatic(const bool isWhite, const uint8_t(*pieces)[13][13]);

		void basicGenerator(std::vector<Move>& moves, uint8_t(*state)[13][13], 
			int x, int y, 
			const int startX, const int startY,
			bool(*visited)[13][13], 
			int remainingSteps, 
			bool turned, bool isWhite, 
			const uint8_t(*pieces)[13][13]);
		void captureGenerator(std::vector<Move>& moves, uint8_t(*state)[13][13], 
			int originX, int originY, 
			int x, int y, 
			const int startX, const int startY,
			bool(*visited)[13][13], 
			int remainingSteps, 
			bool turned, bool isWhite, 
			const uint8_t(*pieces)[13][13]);
	private:
		static MoveGenerator moveGen;
	};
}