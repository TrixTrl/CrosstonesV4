#include "Board.h"
#include "Utility.h"
#include "Zobrist.h"
#include "MoveGenerator.h"

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
	appyMoveReversible(move);
	updateWinValue();
}
void Board::unmakeMove(Move& move)
{
	appyMoveReversible(move);
	gameResult = GameResult::InProgress;
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


u64 Board::bulk_perft(int depth /* >= 1 */, std::function<std::vector<Move>(uint8_t(*)[13][13], bool)> getMoves)
{

	int n_moves, i;
	u64 nodes = 0;

	std::vector<Move> moves = getMoves(&square, isWhiteTurn);
	//MoveGenerator::getMovesStatic(&moves, &square, isWhiteTurn);

	n_moves = moves.size();

	if (depth == 1)
		return (u64)n_moves;

	for (i = 0; i < n_moves; i++) {
		makeMove(moves[i]);
		nodes += bulk_perft(depth - 1, getMoves);
		unmakeMove(moves[i]);
	}
	return nodes;
}

std::string Board::moveToString(Move& move)
{
	if (move.size() == 0)
		return "Null move";

	std::string result = "";
	for (auto& xMove : move)
	{
		// use binary xor value or concrete information if there is context
		std::string action = "";
		const uint8_t fieldBefore = square[xMove.i][xMove.j];
		const uint8_t fieldAfter = fieldBefore ^ xMove.delta;

		if (Piece::height(fieldAfter) != Piece::height(fieldBefore)) {
			action += (Piece::height(fieldAfter) > Piece::height(fieldBefore))
				? "+" : "";

			action += std::to_string(Piece::height(fieldAfter) - Piece::height(fieldBefore));

			action += ((fieldAfter | fieldBefore) & Piece::Black)
				? "B" : "W";
		}
		if ((fieldBefore ^ fieldAfter) & Piece::Red)
			action += "_Red";
		if ((fieldBefore ^ fieldAfter) & Piece::Blue)
			action += "_Blue";

		if (xMove.delta & Piece::setTurnPiece)
			action += "_T";
		if (xMove.delta & Piece::hasTurnPiece)
			action += " _turnPieceCreate";

		result += std::format("\n({}, {}) | {}",
			std::string(1, (char)xMove.i + 'A'),
			13 - xMove.j,
			action
		);
	}
	return result.substr(1);
}