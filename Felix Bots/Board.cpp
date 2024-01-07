#include "Board.h"
#include "Utility.h"
#include "Zobrist.h"

using namespace dc;

void Board::initialize(uint8_t(*state)[13][13], bool isWhiteTurn)
{
	std::memcpy(square, state, sizeof(square));
	this->isWhiteTurn = isWhiteTurn;

	zobristKey = Zobrist::calculateZobristKey(*this);
}


void Board::makeMove(Move& move)
{
	for (int i = 0; i < move.size(); i++) {
		uint8_t pos = move[i].i + 13 * move[i].j;
		zobristKey ^= Zobrist::piecesArray[square[move[i].i][move[i].j] & 0b00111111][pos];

		square[move[i].i][move[i].j] ^= move[i].delta;

		// if turned
		if (move[i].delta & 0b01000000)
		{
			zobristKey ^= Zobrist::turnPiecesArray[Piece::turnPieceIndices[pos]];
		}

		zobristKey ^= Zobrist::piecesArray[square[move[i].i][move[i].j] & 0b00111111][pos];
	}
	isWhiteTurn = !isWhiteTurn;

	zobristKey ^= Zobrist::whiteToMove;
}
void Board::unmakeMove(Move& move)
{
	makeMove(move);
}
