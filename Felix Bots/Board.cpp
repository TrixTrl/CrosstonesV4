#include "Board.h"
#include "Utility.h"
#include "Zobrist.h"

using namespace dc;

void Board::initialize(uint8_t(*state)[13][13], bool isWhiteTurn)
{
	std::memcpy(square, state, sizeof(square));
	this->isWhiteTurn = isWhiteTurn;

	gameResult = GameResult::InProgress;
	zobristKey = Zobrist::calculateZobristKey(*this);
}

void Board::updateWinValue()
{
	gameResult = GameResult::InProgress;
	GameResult result = GameResult::InProgress;

	// Check for win in black base
	bool blackBaseContested = false;
	for (int i = 5; i < 8; i++) {
		for (int j = 0; j < 4; j++) {
			if (Piece::isTower(square[i][j])) {
				if (Piece::isWhite(square[i][j])) {
					blackBaseContested = true;
				}
				else {
					goto blackBaseSafe;
				}
			}
		}
	}
	if (blackBaseContested)
	{
		result = GameResult::WhiteHasWon;
	}
blackBaseSafe:

	// Check for win in white base
	bool whiteBaseContested = false;
	for (int i = 5; i < 8; i++) {
		for (int j = 0; j < 4; j++) {
			if (Piece::isTower(square[i][12 - j])) {
				if (Piece::isWhite(square[i][12 - j])) {
					goto whiteBaseSafe;
				}
				else {
					whiteBaseContested = true;
				}
			}
		}
	}
	if (whiteBaseContested)
	{
		// Check for draw by double win
		if (result == GameResult::WhiteHasWon)
		{
			gameResult = GameResult::Draw;
			return;
		}
		result = GameResult::BlackHasWon;
	}
whiteBaseSafe:

	// Now we check for port wins
	
	// This port index will stay active over all loops (we absolutely only want to visit each port once)
	uint8_t portIndex = 0;
	// Same here, although this one stores the index within the port (from 0 to 11)
	uint8_t localPortIndex = 0;

	// Check the first port
	// It will restrict the options if soemone can have a Port win.
	bool portWinCandidateIsWhite = false;

	for (; localPortIndex < 12; localPortIndex++)
	{
		uint8_t pos = Piece::portIndices[portIndex][localPortIndex];
		if (Piece::isTower(square[pos % 13][pos / 13])) {
			if (Piece::isWhite(square[pos % 13][pos / 13])) {
				portWinCandidateIsWhite = true;
			}
			else {
				portWinCandidateIsWhite = false;
			}
			goto candidateElimination;
		}
	}
	// Here, the port was empty, so we're done
	gameResult = result;
	return;

	// Here, we continue to see if the one of the players that can win *has* won.
	// We will start by comparing the same square again (a small overhead, but less verbose)
	// We will search until we see that there is a port where the candidate has no pieces or the enemy has a piece in
candidateElimination:

	bool candidateInCurrentPort = false;
	for (; portIndex < 4; portIndex++)
	{
		for (; localPortIndex < 12; localPortIndex++)
		{
			uint8_t pos = Piece::portIndices[portIndex][localPortIndex];
			if (Piece::isTower(square[pos % 13][pos / 13])) {
				if (Piece::isWhite(square[pos % 13][pos / 13]) == portWinCandidateIsWhite) {
					// If the tower is of the candidate
					candidateInCurrentPort = true;
				}
				else
				{
					// If the tower is of the enemy, port win is prevented
					gameResult = result;
					return;
				}
			}
		}
		if (!candidateInCurrentPort)
		{
			// Here the port was empty, so no one has port win
			gameResult = result;
			return;
		}
		// Reset for check of next port
		candidateInCurrentPort = 0;
		localPortIndex = 0;
	}

	// If we reach this far, that means the candidate has won by ports
	if (portWinCandidateIsWhite)
	{
		// Handle draw by double win
		if (result == GameResult::BlackHasWon)
		{
			gameResult = GameResult::Draw;
			return;
		}
		gameResult = GameResult::WhiteHasWon;
		return;
	}
	else
	{
		// Handle draw by double win
		if (result == GameResult::WhiteHasWon)
		{
			gameResult = GameResult::Draw;
			return;
		}
		gameResult = GameResult::BlackHasWon;
		return;
	}
}

void Board::makeMove(Move& move)
{
	updateWinValue();
	appyMoveReversible(move);
}
void Board::unmakeMove(Move& move)
{
	gameResult = GameResult::InProgress;
	appyMoveReversible(move);
}

void Board::appyMoveReversible(Move& move)
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
