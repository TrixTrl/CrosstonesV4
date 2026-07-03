#include "Zobrist.h"
#include "globals/Piece.h"
#include <random>

using namespace dc;

//execute the initializer
uint64_t Zobrist::whiteToMove = 0;
uint64_t Zobrist::piecesArray[54][169];
uint64_t Zobrist::turnPiecesArray[20];
std::mt19937_64 Zobrist::gen64;
Zobrist::_init Zobrist::_initializer;

// Calculate zobrist key from current board position.
// NOTE: this function is slow and should only be used when the board is initially set up from a string.
// During search, the key should be updated incrementally instead.
uint64_t Zobrist::calculateZobristKey(BotBoard& board)
{
	uint64_t zobristKey = 0;

	for (int x = 0; x < 13; x++)
	{
		for (int y = 0; y < 13; y++)
		{
			if ((x % 2) + (y % 2) == 2) continue;
			uint8_t piece = board.square[x][y] & 0b00111111;

			if (piece != Piece::None)
			{
				int squareIndex = x + 13 * y;
				zobristKey ^= piecesArray[piece][squareIndex];
			}
		}
	}

	for (int i = 0; i < 20; i++)
	{
		int pos = Piece::turnPiecePositions[i];
		int piece = board.square[pos % 13][pos / 13];
		if (piece & Piece::setTurnPiece)
		{
			zobristKey ^= turnPiecesArray[i];
		}
	}

	if (board.isWhiteTurn)
		zobristKey ^= whiteToMove;

	return zobristKey;
}

uint64_t Zobrist::randomUnsigned64BitNumber()
{
	return gen64();
}