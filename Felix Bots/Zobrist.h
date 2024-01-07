#pragma once
#include <cstdint>
#include "Board.h"
#include "../Globals/Piece.h"
#include <random>

namespace dc
{
	static class Zobrist
	{
	public:
		// Helper class for the calculation of zobrist hash.
		// This is a single 64bit value that (non-uniquely) represents the current state of the game.

		// It is mainly used for quickly detecting positions that have already been evaluated, to avoid
		// potentially performing lots of duplicate work during game search.

		// Random numbers are generated for each aspect of the game state, and are used for calculating the hash:

		// piece type, colour, square index
		static uint64_t piecesArray[54][169];

		// turn pieces in constant order
		// only applied if turn piece is set (vertical)
		static uint64_t turnPiecesArray[20];
		static uint64_t whiteToMove;

		static uint64_t calculateZobristKey(Board& board);
		static uint64_t randomUnsigned64BitNumber();

		static class _init
		{
		public:
			_init() 
			{
				for (int squareIndex = 0; squareIndex < 169; squareIndex++)
				{
					for (const int piece : Piece::pieceIndices)
					{
						piecesArray[piece][squareIndex] = randomUnsigned64BitNumber();
					}
				}
				for (int i = 0; i < 20; i++)
				{
					turnPiecesArray[i] = randomUnsigned64BitNumber();
				}
				whiteToMove = randomUnsigned64BitNumber();
			}
		} _initializer;

	private:
		static std::mt19937_64 gen64;
	};
}