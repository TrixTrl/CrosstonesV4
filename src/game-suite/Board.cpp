#include "Board.h"
#include <iostream>
#include <string>
#include "globals/Piece.h"
#include <cstdint>
#include <sstream>
#include <iomanip>

#include "fix_win32_compatibility.h"
#include <WinUser.h>

#define BACKGROUND "\x1b[43m\x1b[38;5;52m"
#define RESET "\x1b[m"
#define BLACK "\x1b[38;5;0m"
#define WHITE "\x1b[37m"
#define RED "\x1b[38;5;9mR"
#define BLUE "\x1b[38;5;12mB"

#define DEBUG_PRINTING false

Board::Board()
{
	for (int i = 0; i < 13; i++)
	{
		for (int j = 0; j < 13; j++)
		{
			gamePos[i][j] = 0;
		}
	}

	// Ports
	// gamePos[0][0] = hasTurnPiece;
	gamePos[2][0] = hasTurnPiece;
	gamePos[0][2] = hasTurnPiece;
	gamePos[2][2] = hasTurnPiece;

	gamePos[10][0] = hasTurnPiece;
	// gamePos[12][0] = hasTurnPiece;
	gamePos[10][2] = hasTurnPiece;
	gamePos[12][2] = hasTurnPiece;

	// gamePos[0][12] = hasTurnPiece;
	gamePos[2][12] = hasTurnPiece;
	gamePos[0][10] = hasTurnPiece;
	gamePos[2][10] = hasTurnPiece;

	gamePos[10][12] = hasTurnPiece;
	// gamePos[12][12] = hasTurnPiece;
	gamePos[10][10] = hasTurnPiece;
	gamePos[12][10] = hasTurnPiece;

	// Bases
	gamePos[6][0] = hasTurnPiece;
	gamePos[6][2] = hasTurnPiece;
	gamePos[6][10] = hasTurnPiece;
	gamePos[6][12] = hasTurnPiece;

	// Gates
	gamePos[2][6] = hasTurnPiece;
	gamePos[4][6] = hasTurnPiece;
	gamePos[8][6] = hasTurnPiece;
	gamePos[10][6] = hasTurnPiece;
}

void Board::rst(std::bitset<3> &tps)
{
	uint8_t turnPieceMask = 0b10111111;
	uint8_t resetMask = 0b11000000;

	// gamePos[0][0] = turnPieceMask & gamePos[0][0] | (tps[0] * setTurnPiece);
	gamePos[2][0] = turnPieceMask & gamePos[2][0] | (tps[0] * setTurnPiece);
	gamePos[0][2] = turnPieceMask & gamePos[0][2] | (tps[0] * setTurnPiece);
	gamePos[2][2] = turnPieceMask & gamePos[2][2] | (tps[0] * setTurnPiece);

	gamePos[10][0] = turnPieceMask & gamePos[10][0] | (tps[0] * setTurnPiece);
	// gamePos[12][0] = turnPieceMask & gamePos[12][0] | (tps[0] * setTurnPiece);
	gamePos[10][2] = turnPieceMask & gamePos[10][2] | (tps[0] * setTurnPiece);
	gamePos[12][2] = turnPieceMask & gamePos[12][2] | (tps[0] * setTurnPiece);

	// gamePos[0][12] = turnPieceMask & gamePos[0][12] | (tps[0] * setTurnPiece);
	gamePos[2][12] = turnPieceMask & gamePos[2][12] | (tps[0] * setTurnPiece);
	gamePos[0][10] = turnPieceMask & gamePos[0][10] | (tps[0] * setTurnPiece);
	gamePos[2][10] = turnPieceMask & gamePos[2][10] | (tps[0] * setTurnPiece);

	gamePos[10][12] = turnPieceMask & gamePos[10][12] | (tps[0] * setTurnPiece);
	// gamePos[12][12] = turnPieceMask & gamePos[12][12] | (tps[0] * setTurnPiece);
	gamePos[10][10] = turnPieceMask & gamePos[10][10] | (tps[0] * setTurnPiece);
	gamePos[12][10] = turnPieceMask & gamePos[12][10] | (tps[0] * setTurnPiece);

	gamePos[6][0] = turnPieceMask & gamePos[6][0] | (tps[1] * setTurnPiece);
	gamePos[6][2] = turnPieceMask & gamePos[6][2] | (tps[1] * setTurnPiece);
	gamePos[6][10] = turnPieceMask & gamePos[6][10] | (tps[1] * setTurnPiece);
	gamePos[6][12] = turnPieceMask & gamePos[6][12] | (tps[1] * setTurnPiece);

	gamePos[2][6] = turnPieceMask & gamePos[2][6] | (tps[2] * setTurnPiece);
	gamePos[4][6] = turnPieceMask & gamePos[4][6] | (tps[2] * setTurnPiece);
	gamePos[8][6] = turnPieceMask & gamePos[8][6] | (tps[2] * setTurnPiece);
	gamePos[10][6] = turnPieceMask & gamePos[10][6] | (tps[2] * setTurnPiece);

	for (int i = 0; i < 13; i++)
	{
		for (int j = 0; j < 13; j++)
		{
			gamePos[i][j] &= resetMask;
		}
	}

	gamePos[5][0] |= 3 | Piece::Black;
	gamePos[7][0] |= 3 | Piece::Black;
	gamePos[5][2] |= 3 | Piece::Black;
	gamePos[7][2] |= 3 | Piece::Black;

	gamePos[5][10] |= 3 | Piece::White;
	gamePos[7][10] |= 3 | Piece::White;
	gamePos[5][12] |= 3 | Piece::White;
	gamePos[7][12] |= 3 | Piece::White;

	gamePos[0][0] |= Piece::Blue | 1;
	gamePos[12][0] |= Piece::Blue | 1;
	gamePos[0][12] |= Piece::Blue | 1;
	gamePos[12][12] |= Piece::Blue | 1;
	gamePos[6][6] |= Piece::Blue | 1;

	gamePos[0][6] |= Piece::Red | 1;
	gamePos[12][6] |= Piece::Red | 1;

	/*gamePos[5][10] |= Piece::White | 1;
	gamePos[6][9] |= Piece::White | 5;
	gamePos[2][8] |= Piece::White | 1;
	gamePos[5][4] |= Piece::White | 2 | Piece::Blue;
	gamePos[8][4] |= Piece::White | 1;
	gamePos[6][3] |= Piece::White | 1;
	gamePos[8][3] |= Piece::White | 3 | Piece::Red;
	gamePos[6][8] |= Piece::Black | 2 | Piece::Blue;
	gamePos[6][6] |= Piece::Black | 4 | Piece::Red;
	gamePos[3][2] |= Piece::Black | 2 | Piece::Blue;
	gamePos[6][2] |= Piece::Black | 3;*/

	/*gamePos[5][2] |= 4 | Piece::Blue | Piece::White;
	gamePos[6][2] |= 3 | Piece::Black;
	gamePos[7][2] |= 2 | Piece::Black;
	gamePos[6][3] |= 3 | Piece::White;*/
}

void Board::wipe()
{
	for (int i = 0; i < 13; i++)
	{
		for (int j = 0; j < 13; j++)
		{
			gamePos[i][j] &= 0b10000000;
		}
	}
}

void debugPrint(std::string str)
{
	if (!DEBUG_PRINTING)
		return;
	std::string temp = std::string(str.begin(), str.end());
	LPCSTR wideString = temp.c_str();
	OutputDebugString(wideString);
}

void forceDebugPrint(std::string str)
{
	std::string temp = std::string(str.begin(), str.end());
	LPCSTR wideString = temp.c_str();
	OutputDebugString(wideString);
}

std::vector<std::vector<Board::xMove>> Board::getMoves(bool isWhite) const
{
	std::vector<std::vector<xMove>> moves = std::vector<std::vector<xMove>>();
	moves.reserve(700);

	for (int i = 0; i < 13; i++)
	{
		for (int j = 0; j < 13; j++)
		{
			uint8_t piece = gamePos[i][j];
			if (Piece::height(piece) == 0 || Piece::isAddOn(piece))
				continue;
			if (Piece::isWhiteTower(piece) == !isWhite)
				continue;

			GamePosition gamePosCopy = gamePos;

			VisitedMap visited;
			visited[i][j] = true;

			if (DEBUG_PRINTING)
				debugPrint("X : " + std::to_string(i) + " : " + std::to_string(j) + " | " + std::to_string(moves.size()) + "\n");

			basicGenerator(moves, gamePosCopy, i, j, visited, Piece::maxSteps(piece), false, isWhite);
		}
	}

	return moves;
}

void Board::basicGenerator(
	std::vector<std::vector<xMove>>& moves, 
	const GamePosition& state, 
	int x, int y, VisitedMap& visited, int remainingSteps, bool turned, bool isWhite
) const
{
	std::vector<Board::xMove> move = std::vector<Board::xMove>();
	move.reserve(10);
	for (int i = 0; i < 13; i++)
	{
		for (int j = 0; j < 13; j++)
		{
			if (gamePos[i][j] == state[i][j])
				continue;
			move.emplace_back(xMove{ i, j, (uint8_t)(gamePos[i][j] ^ state[i][j]) }); // Creation of xor lists for moves
		}
	}
	if (moves.size() == 0 || move.size() > 0)
	{
		moves.emplace_back(move);
	}

	if (!turned && ((state[x][y] & hasTurnPiece) != 0))
	{ // Turn in place if we can and haven't yet
		GamePosition boardCopy = state;
		boardCopy[x][y] ^= setTurnPiece;
		VisitedMap visitedCopy = visited;

		if (DEBUG_PRINTING)
			debugPrint("T : " + std::to_string(x) + " : " + std::to_string(y) + " | " + std::to_string(moves.size()) + "\n");

		basicGenerator(moves, boardCopy, x, y, visitedCopy, remainingSteps, true, isWhite);
	}

	if (remainingSteps == 0)
		return;

	/*
		0
	   3 1
		2
	*/
	for (int d = 0; d < 4; d++)
	{ // Loop through the 4 possible directions
		int destX = x + dx[d];
		int destY = y + dy[d];
		uint8_t piece = state[x][y];
		uint8_t dest = state[destX][destY];

		if (destX < 0 || destX > 12 || destY < 0 || destY > 12) continue; //borders check 
		if (destX % 2 == 1 && destY % 2 == 1) continue; //voids check
		if (visited[destX][destY]) continue; // Bounds and revisiting check
		if (Piece::isBlockedByGate(piece, d)) continue;
		if (Piece::isBlockedByGate(dest, d)) continue;

		if ((dest & 0b00111111) == 0)
		{ // Most basic case : empty target square
			for (int splitOff = 1; splitOff <= Piece::height(piece); splitOff++)
			{ // number of moved pieces
				GamePosition boardCopy = state;

				if (splitOff == Piece::height(piece))
				{
					boardCopy[x][y] &= 0b11000000;
				}
				else
				{
					boardCopy[x][y] -= splitOff;
					if (Piece::hasAddOn(piece) && splitOff == Piece::height(piece) - 1)
					{
						boardCopy[x][y] &= 0b11011111; // erase trailing color
					}
				}
				boardCopy[destX][destY] |= (uint8_t)((piece) & 0b00100000 | splitOff);
				if (splitOff == Piece::height(piece))
				{
					boardCopy[destX][destY] |= (uint8_t)((piece) & 0b00011000); // addons, only if  we're moving the whole tower
				}

				VisitedMap visitedCopy = visited;
				visitedCopy[destX][destY] = true;

				if (DEBUG_PRINTING)
					debugPrint("O : " + std::to_string(destX) + " : " + std::to_string(destY) + " | " + std::to_string(moves.size()) + "\n");

				basicGenerator(moves, boardCopy, destX, destY, visitedCopy, remainingSteps - 1, false, isWhite);
			}
		}
		else if (Piece::height(dest) > 0 && (Piece::isAddOn(dest) ? true : (Piece::colour(dest) == Piece::colour(piece))))
		{ // merging
			for (int splitOff = 1; (splitOff <= Piece::height(piece)) && (splitOff <= 5 - Piece::height(dest)); splitOff++)
			{
				if (splitOff == Piece::height(piece) && Piece::hasAddOn(piece))
					continue;

				GamePosition boardCopy = state;

				if (splitOff == Piece::height(piece))
				{
					boardCopy[x][y] &= 0b11000000;
				}
				else
				{
					boardCopy[x][y] -= splitOff;
					if (Piece::hasAddOn(piece) && splitOff == Piece::height(piece) - 1)
					{
						boardCopy[x][y] &= 0b11011111; // erase trailing color
					}
				}
				boardCopy[destX][destY] |= (uint8_t)((piece) & 0b00100000); // color info
				boardCopy[destX][destY] += splitOff;
				if (splitOff == Piece::height(piece))
				{
					boardCopy[destX][destY] |= (uint8_t)((piece) & 0b00011000); // addons, only if we're moving the whole tower
				}

				VisitedMap visitedCopy = visited;
				visitedCopy[destX][destY] = true;

				if (DEBUG_PRINTING)
					debugPrint("M : " + std::to_string(destX) + " : " + std::to_string(destY) + " | " + std::to_string(moves.size()) + "\n");

				basicGenerator(moves, boardCopy, destX, destY, visitedCopy, 0, false, isWhite);
			}
		}

		if (Piece::height(dest) > 0 && Piece::height(dest) <= Piece::height(piece) && (Piece::isAddOn(dest) ? false : (Piece::colour(dest) == Piece::colour(piece))))
		{ // pushing (smaller than the pushing piece, not an addon and the same color)
			int offset = 2;
			bool pushValid = false;
			int pushHeight = Piece::height(dest);

			/*
				Continue checking in the direction of movement until pushing is either posible or it isn't
				Offset is the offset from the original piece
				We loop until we hit a wall or something we can't push (hopefully)
			*/

			while (true)
			{
				int pushX = x + dx[d] * offset;
				int pushY = y + dy[d] * offset;

				if (pushX < 0 || pushX > 12 || pushY < 0 || pushY > 12)
					break;

				uint8_t pushPiece = state[pushX][pushY];

				if ((pushPiece & hasTurnPiece) != 0)
				{ // Obey turn pieces
					if (((pushPiece & setTurnPiece)) == (d % 2) * setTurnPiece)
						break; // This feels deeply cursed
				}
				if (Piece::height(pushPiece) == 0)
				{
					pushValid = true;
					break;
				}
				if (Piece::height(pushPiece) <= pushHeight && (Piece::isAddOn(pushPiece) ? false : (Piece::colour(pushPiece) == Piece::colour(piece))))
				{
					offset++;
					pushHeight = Piece::height(pushPiece);
				}
				else
				{
					break;
				}
			}
			if (!pushValid)
				continue;

			GamePosition boardCopy = state;

			for (int o = offset; o > 0; o--)
			{ // loop backwards from the end of the push chain and copy the pieces over
				int destX = x + dx[d] * o;
				int destY = y + dy[d] * o;
				int sourceX = x + dx[d] * (o - 1);
				int sourceY = y + dy[d] * (o - 1);

				boardCopy[destX][destY] |= boardCopy[sourceX][sourceY] & 0b00111111;
				boardCopy[sourceX][sourceY] &= 0b11000000;
			}

			VisitedMap visitedCopy = visited;
			visitedCopy[destX][destY] = true;

			if (DEBUG_PRINTING)
				debugPrint("P : " + std::to_string(destX) + " : " + std::to_string(destY) + " | " + std::to_string(moves.size()) + "\n");

			basicGenerator(moves, boardCopy, destX, destY, visitedCopy, remainingSteps - 1, false, isWhite);
		}

		if (remainingSteps >= 2 && Piece::isTower(dest) && Piece::colour(dest) != Piece::colour(piece))
		{ // capturing
		  // making sure we have enough moves left to be able to capture
			if (!Piece::isBlue(piece) && Piece::height(dest) > Piece::height(piece))
				continue; // capturing height check

			// Logic: figure out all ways to capture and then generate all splitting options at the end to not redo the work over and over again (this might be something good to cache when trying to optimize)

			GamePosition boardCopy = state;

			boardCopy[destX][destY] &= 0b11000000; // remove piece we're capturing
			VisitedMap visitedCopy = visited;
			visitedCopy[destX][destY] = true;
			if (DEBUG_PRINTING)
				debugPrint("C : " + std::to_string(destX) + " : " + std::to_string(destY) + " | " + std::to_string(moves.size()) + "\n");
			captureGenerator(moves, boardCopy, x, y, destX, destY, visitedCopy, remainingSteps - 1, false, isWhite);
		}
	}
}

void Board::captureGenerator(
	std::vector<std::vector<xMove>>& moves, 
	const GamePosition& state, 
	int originX, int originY, int x, int y, 
	VisitedMap& visited, int remainingSteps, bool turned, bool isWhite
) const
{
	/*
		0
	   3 1
		2
	*/
	uint8_t origin = state[originX][originY];

	if (!turned && ((state[x][y] & hasTurnPiece) != 0))
	{ // Turn in place if we can and haven't yet
		GamePosition boardCopy = state;
		boardCopy[x][y] ^= setTurnPiece;

		VisitedMap visitedCopy = visited;

		if (DEBUG_PRINTING)
			debugPrint("CT : " + std::to_string(x) + " : " + std::to_string(y) + " | " + std::to_string(moves.size()) + "\n");

		captureGenerator(moves, boardCopy, originX, originY, x, y, visitedCopy, remainingSteps, true, isWhite);
	}

	for (int d = 0; d < 4; d++)
	{ // Loop through the 4 possible directions
		uint8_t piece = state[x][y];
		if ((piece & hasTurnPiece) != 0)
		{ // Obey turn pieces
			if (((piece & setTurnPiece)) == (d % 2) * setTurnPiece)
				continue; // This feels deeply cursed
		}
		int i = x + dx[d];
		int j = y + dy[d];
		uint8_t dest = state[i][j];
		if (i < 0 || i > 12 || j < 0 || j > 12)
			continue;
		if (i % 2 == 1 && j % 2 == 1)
			continue;
		if (visited[i][j])
			continue; // Bounds and revisiting check
		if ((dest & hasTurnPiece) != 0 && (((dest & setTurnPiece)) == (d % 2) * setTurnPiece))
			continue;

		// piece is the position where an enemy piece used to be that we are currently taking, this value has already been wiped
		// dest is the piece we are now looking to move to, either to complete the capture or to continue the chain
		// the original piece doing the capturing still exists at originX, originY in the state array
		// splitting will be done once the end of a capture chain has been found

		// ending the chain on an empty square or a friendly tower or a single addon to merge onto
		if (Piece::height(dest) == 0 || (Piece::isTower(dest) && Piece::colour(dest) == Piece::colour(origin)) || Piece::isAddOn(dest))
		{

			// We have to split less than the full tower, except if the height is 1 and we can't go over the merge limit
			for (int splitOff = 1; (Piece::height(origin) == 1 ? splitOff <= 1 : (splitOff < Piece::height(origin))) && (splitOff <= 5 - Piece::height(dest)); splitOff++)
			{
				GamePosition boardCopy = state;

				if (Piece::height(origin) == 1)
				{
					boardCopy[originX][originY] &= 0b11000000; // wipe the orign only if the height is one
				}
				else
				{
					boardCopy[originX][originY] -= splitOff;
					if (Piece::isAddOn(boardCopy[originX][originY]))
						boardCopy[originX][originY] &= 0b11011111; // if we left a single addon, remove the color
				}

				boardCopy[i][j] += splitOff;
				boardCopy[i][j] |= Piece::colour(origin); // always copy color and add height as that should be allowed based on the only way this can be called

				VisitedMap visitedCopy = visited;
				visitedCopy[i][j] = true;

				if (DEBUG_PRINTING)
					debugPrint("CM : " + std::to_string(i) + " : " + std::to_string(j) + " | " + std::to_string(moves.size()) + "\n");
				basicGenerator(moves, boardCopy, i, j, visitedCopy, 0, false, isWhite); // do last move
																						// TODO: Wrong ruleset bug, verify this change is correct
																						//  //if dest is empty, continue the move if possible
																						// basicGenerator(moves, &boardCopy, i, j, &visitedCopy, Piece::height(dest) == 0 ? remainingSteps - 1 : 0, false, isWhite);
			}
		}
		else if (remainingSteps + Piece::isBlue(origin) > 1 && Piece::isTower(dest) && Piece::colour(origin) != Piece::colour(dest))
		{
			if (!Piece::isBlue(origin) && Piece::height(dest) > Piece::height(origin))
				continue; // capturing height check
			GamePosition boardCopy = state;
			boardCopy[i][j] &= 0b11000000; // remove piece we're capturing

			VisitedMap visitedCopy = visited;
			visitedCopy[i][j] = true;

			debugPrint("CC : " + std::to_string(i) + " : " + std::to_string(j) + " | " + std::to_string(moves.size()) + "\n");
			captureGenerator(moves, boardCopy, originX, originY, i, j, visitedCopy, remainingSteps - !Piece::isBlue(origin), false, isWhite); // only decrement moves if the capturing piece doesn't have a blue addon
		}
	}
}

void Board::unsafeMakeMove(const std::vector<xMove>& move)
{
	unsafeApplyMove(gamePos, move);
}

void Board::unsafeApplyMove(GamePosition& position, const std::vector<xMove>& move)
{
	for (int i = 0; i < move.size(); i++)
	{
		position[move[i].i][move[i].j] ^= move[i].delta;
	}
}

int Board::makeMove(const std::vector<xMove>& move, bool isWhiteTurn)
{
	GamePosition newState = gamePos;
	unsafeApplyMove(newState, move);

	return makeMove(newState, isWhiteTurn);
}

int Board::makeMove(GamePosition& newState, bool isWhiteTurn)
{
	std::vector<std::vector<Board::xMove>> moves = getMoves(isWhiteTurn);

	for (int i = 0; i < moves.size(); i++)
	{
		GamePosition boardCopy = gamePos;
		unsafeApplyMove(boardCopy, moves[i]);

		if (newState == boardCopy)
		{
			gamePos = newState;

			forceDebugPrint(dumpPosition());
			forceDebugPrint("\n");

			gameRecord.emplace_back(std::to_string(i) + " ");

			dumpGame();

			return i == 0 ? 0 : 1;
		}
	}
	return -1;
}

Board::winValue Board::gameOver(bool isWhite) // last player to make a move
{
	// Check for full eliminations, if yes, that player wins
	// Check for base wins
	// Check for port wins
	// Check to see if a player has one and make sure the right one wins, incuding the Bridge Rule

	bool whiteNoPieces = true;
	bool blackNoPieces = true;
	for (int i = 0; i < 13 && (whiteNoPieces || blackNoPieces); i++)
	{
		for (int j = 0; j < 13 && (whiteNoPieces || blackNoPieces); j++)
		{
			if (Piece::isTower(gamePos[i][j]))
			{
				if (Piece::isWhite(gamePos[i][j]))
				{
					whiteNoPieces = false;
				}
				else
				{
					blackNoPieces = false;
				}
			}
		}
	}
	if (whiteNoPieces)
		return winValue::black;
	if (blackNoPieces)
		return winValue::white;
	// There can be no draw if one side has no pieces left so we don't need any extra checks

	bool bases[2][2] = {{false, false}, {false, false}}; //[0] white   [1] black   [x][0] white base   [x][1] black base
	for (int i = 5; i < 8; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (Piece::isTower(gamePos[i][j]))
			{
				if (Piece::isWhite(gamePos[i][j]))
				{
					bases[0][1] = true; // black base white piece
				}
				else
				{
					bases[1][1] = true; // black base black piece
				}
			}

			if (Piece::isTower(gamePos[i][12 - j]))
			{
				if (Piece::isWhite(gamePos[i][12 - j]))
				{
					bases[0][0] = true; // white base white piece
				}
				else
				{
					bases[1][0] = true; // white base black piece
				}
			}
		}
	}

	bool whiteWin = false;
	bool blackWin = false;
	if (bases[0][1] && !bases[1][1])
		whiteWin = true;
	if (bases[1][0] && !bases[0][0])
		blackWin = true;

	bool portControll[4][2] = {false, false, false, false, false, false, false, false}; //[top left, top right, bottom left, bottom right],[white, black]
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			for (int corner = 0; corner < 4; corner++)
			{
				uint8_t piece;
				switch (corner)
				{
				case 0:
					piece = gamePos[i][j];
					break;
				case 1:
					piece = gamePos[12 - i][j];
					break;
				case 2:
					piece = gamePos[i][12 - j];
					break;
				case 3:
					piece = gamePos[12 - i][12 - j];
					break;
				}
				if (Piece::isTower(piece))
				{
					portControll[corner][1 - Piece::isWhiteTower(piece)] = true;
				}
			}
		}
	}
	int portNumbers[2] = {0, 0};
	for (int i = 0; i < 4; i++)
	{
		portNumbers[0] += portControll[i][0];
		portNumbers[1] += portControll[i][1];
	}
	if (portNumbers[0] == 4 && portNumbers[1] == 0)
		whiteWin = true;
	if (portNumbers[1] == 4 && portNumbers[0] == 0)
		blackWin = true;
	if (!(whiteWin || blackWin))
	{
		return winValue::none;
	}
	if (whiteWin && !blackWin)
		return winValue::white;
	if (!whiteWin && blackWin)
		return winValue::black;
	return isWhite ? winValue::white : winValue::black;
}

void Board::copyPositionTo(GamePosition& dest) const
{
	dest = gamePos;
}
void Board::copyPositionTo(uint8_t(*dest)[13][13]) const
{
    for (int i = 0; i < 13; i++)
    {
        for (int j = 0; j < 13; j++)
        {
            (*dest)[i][j] = gamePos[i][j];
        }
    }
}

GamePosition Board::positionCopy() const 
{
	return gamePos;
}

std::string Board::dumpPosition() const
{
	std::string turnPieceStr = "";
	std::string str = "";

	for (int i = 0; i < 13; i++)
	{
		for (int j = 0; j < 13; j++)
		{
			uint8_t piece = gamePos[i][j];

			if (!((piece & hasTurnPiece) == 0))
			{
				turnPieceStr += ((piece & setTurnPiece) == 0) ? "0" : "1";
			}

			if (Piece::height(piece) == 0)
				continue;
			str += (Piece::hasAddOn(piece) ? (Piece::isRed(piece) ? "r" : "b") : "-");
			str += Piece::isAddOn(piece) ? "-" : (Piece::isWhiteTower(piece) ? "W" : "B");
			str += std::to_string(Piece::height(piece));

			std::ostringstream ss1;
			ss1 << std::setw(2) << std::setfill('0') << i;
			std::ostringstream ss2;
			ss2 << std::setw(2) << std::setfill('0') << j;

			str += ss1.str();
			str += ss2.str();

			str += " ";
		}
	}
	return str + turnPieceStr;
}

void Board::loadPosition(std::string str)
{
	wipe();

	std::string delimiter = " ";

	size_t pos = 0;
	std::string token;
	while ((pos = str.find(delimiter)) != std::string::npos)
	{
		token = str.substr(0, pos);
		uint8_t piece = 0;
		if (token[0] == 'b')
			piece |= Piece::Blue;
		if (token[0] == 'r')
			piece |= Piece::Red;
		if (token[1] == 'B')
			piece |= Piece::Black;
		piece += token[2] - '0';
		int x = 10 * (token[3] - '0') + (token[4] - '0');
		int y = 10 * (token[5] - '0') + (token[6] - '0');
		gamePos[x][y] |= piece;

		str.erase(0, pos + delimiter.length());
	}
	token = str;
	int n = 0;
	for (int i = 0; i < 13; i++)
	{
		for (int j = 0; j < 13; j++)
		{
			if ((gamePos[i][j] & hasTurnPiece) == 0)
				continue;
			gamePos[i][j] |= (token[n] == '1') ? setTurnPiece : 0;
			n++;
		}
	}
}

void Board::dumpGame()
{

	forceDebugPrint("\nGame record:\n\n");
	for (int i = 0; i < gameRecord.size(); i++)
	{
		forceDebugPrint(gameRecord[i]);
	}
	forceDebugPrint("\n\n");
}
