#include "BoardState.h"
#include <iostream>
#include <string>
#include "Globals/Piece.h"
#include <cstdint>
#include <sstream>
#include <iomanip>

#include <Windows.h>
#include <WinUser.h>

#define BACKGROUND "\x1b[43m\x1b[38;5;52m"
#define RESET "\x1b[m"
#define BLACK "\x1b[38;5;0m"
#define WHITE "\x1b[37m"
#define RED "\x1b[38;5;9mR"
#define BLUE "\x1b[38;5;12mB"

#define DEBUG_PRINTING false

BoardState::BoardState() {
	for (int i = 0; i < 13; i++) { for (int j = 0; j < 13; j++) { pieces[i][j] = 0; } }

	//Ports
	//pieces[0][0] = hasTurnPiece;
	pieces[2][0] = hasTurnPiece;
	pieces[0][2] = hasTurnPiece;
	pieces[2][2] = hasTurnPiece;

	pieces[10][0] = hasTurnPiece;
	//pieces[12][0] = hasTurnPiece;
	pieces[10][2] = hasTurnPiece;
	pieces[12][2] = hasTurnPiece;

	//pieces[0][12] = hasTurnPiece;
	pieces[2][12] = hasTurnPiece;
	pieces[0][10] = hasTurnPiece;
	pieces[2][10] = hasTurnPiece;

	pieces[10][12] = hasTurnPiece;
	//pieces[12][12] = hasTurnPiece;
	pieces[10][10] = hasTurnPiece;
	pieces[12][10] = hasTurnPiece;

	//Bases
	pieces[6][0] = hasTurnPiece;
	pieces[6][2] = hasTurnPiece;
	pieces[6][10] = hasTurnPiece;
	pieces[6][12] = hasTurnPiece;

	//Gates
	pieces[2][6] = hasTurnPiece;
	pieces[4][6] = hasTurnPiece;
	pieces[8][6] = hasTurnPiece;
	pieces[10][6] = hasTurnPiece;
}


void BoardState::rst(std::bitset<3>& tps)
{
	uint8_t turnPieceMask = 0b10111111;
	uint8_t resetMask = 0b11000000;

	pieces[0][0] = turnPieceMask & pieces[0][0] | (tps[0] * setTurnPiece);
	pieces[2][0] = turnPieceMask & pieces[2][0] | (tps[0] * setTurnPiece);
	pieces[0][2] = turnPieceMask & pieces[0][2] | (tps[0] * setTurnPiece);
	pieces[2][2] = turnPieceMask & pieces[2][2] | (tps[0] * setTurnPiece);

	pieces[10][0] = turnPieceMask & pieces[10][0] | (tps[0] * setTurnPiece);
	pieces[12][0] = turnPieceMask & pieces[12][0] | (tps[0] * setTurnPiece);
	pieces[10][2] = turnPieceMask & pieces[10][2] | (tps[0] * setTurnPiece);
	pieces[12][2] = turnPieceMask & pieces[12][2] | (tps[0] * setTurnPiece);

	pieces[0][12] = turnPieceMask & pieces[0][12] | (tps[0] * setTurnPiece);
	pieces[2][12] = turnPieceMask & pieces[2][12] | (tps[0] * setTurnPiece);
	pieces[0][10] = turnPieceMask & pieces[0][10] | (tps[0] * setTurnPiece);
	pieces[2][10] = turnPieceMask & pieces[2][10] | (tps[0] * setTurnPiece);

	pieces[10][12] = turnPieceMask & pieces[10][12] | (tps[0] * setTurnPiece);
	pieces[12][12] = turnPieceMask & pieces[12][12] | (tps[0] * setTurnPiece);
	pieces[10][10] = turnPieceMask & pieces[10][10] | (tps[0] * setTurnPiece);
	pieces[12][10] = turnPieceMask & pieces[12][10] | (tps[0] * setTurnPiece);



	pieces[6][0] = turnPieceMask & pieces[6][0] | (tps[1] * setTurnPiece);
	pieces[6][2] = turnPieceMask & pieces[6][2] | (tps[1] * setTurnPiece);
	pieces[6][10] = turnPieceMask & pieces[6][10] | (tps[1] * setTurnPiece);
	pieces[6][12] = turnPieceMask & pieces[6][12] | (tps[1] * setTurnPiece);



	pieces[2][6] = turnPieceMask & pieces[2][6] | (tps[2] * setTurnPiece);
	pieces[4][6] = turnPieceMask & pieces[4][6] | (tps[2] * setTurnPiece);
	pieces[8][6] = turnPieceMask & pieces[8][6] | (tps[2] * setTurnPiece);
	pieces[10][6] = turnPieceMask & pieces[10][6] | (tps[2] * setTurnPiece);

	for (int i = 0; i < 13; i++) {
		for (int j = 0; j < 13; j++) {
			pieces[i][j] &= resetMask;
		}
	}

	pieces[5][0] |= 3 | Piece::Black;
	pieces[7][0] |= 3 | Piece::Black;
	pieces[5][2] |= 3 | Piece::Black;
	pieces[7][2] |= 3 | Piece::Black;

	pieces[5][10] |= 3 | Piece::White;
	pieces[7][10] |= 3 | Piece::White;
	pieces[5][12] |= 3 | Piece::White;
	pieces[7][12] |= 3 | Piece::White;

	pieces[0][0] |= Piece::Blue | 1;
	pieces[12][0] |= Piece::Blue | 1;
	pieces[0][12] |= Piece::Blue | 1;
	pieces[12][12] |= Piece::Blue | 1;
	pieces[6][6] |= Piece::Blue | 1;

	pieces[0][6] |= Piece::Red | 1;
	pieces[12][6] |= Piece::Red | 1;


	/*pieces[5][10] |= Piece::White | 1;
	pieces[6][9] |= Piece::White | 5;
	pieces[2][8] |= Piece::White | 1;
	pieces[5][4] |= Piece::White | 2 | Piece::Blue;
	pieces[8][4] |= Piece::White | 1;
	pieces[6][3] |= Piece::White | 1;
	pieces[8][3] |= Piece::White | 3 | Piece::Red;
	pieces[6][8] |= Piece::Black | 2 | Piece::Blue;
	pieces[6][6] |= Piece::Black | 4 | Piece::Red;
	pieces[3][2] |= Piece::Black | 2 | Piece::Blue;
	pieces[6][2] |= Piece::Black | 3;*/


	/*pieces[5][2] |= 4 | Piece::Blue | Piece::White;
	pieces[6][2] |= 3 | Piece::Black;
	pieces[7][2] |= 2 | Piece::Black;
	pieces[6][3] |= 3 | Piece::White;*/
}

void BoardState::wipe()
{
	for (int i = 0; i < 13; i++) {
		for (int j = 0; j < 13; j++) {
			pieces[i][j] &= 0b10000000;
		}
	}
}

void BoardState::copyBoard(uint8_t(*dest)[13][13]) const
{
	std::memcpy(dest, &pieces, sizeof(pieces));
}

void debugPrint(std::string str) {
	if (!DEBUG_PRINTING) return;
	std::wstring temp = std::wstring(str.begin(), str.end());
	LPCWSTR wideString = temp.c_str();
	OutputDebugString(wideString);
}

void forceDebugPrint(std::string str) {
	std::wstring temp = std::wstring(str.begin(), str.end());
	LPCWSTR wideString = temp.c_str();
	OutputDebugString(wideString);
}

std::shared_ptr<std::vector<std::vector<BoardState::xMove>>> BoardState::getMoves(bool isWhite) const {
	std::shared_ptr<std::vector<std::vector<xMove>>> moves = std::make_shared<std::vector<std::vector<xMove>>>();
	moves->reserve(700);

	for (int i = 0; i < 13; i++) {
		for (int j = 0; j < 13; j++) {
			uint8_t piece = pieces[i][j];
			if (Piece::height(piece) == 0 || Piece::isAddOn(piece)) continue;
			if (Piece::isWhiteTower(piece) == !isWhite) continue;

			uint8_t boardCopy[13][13];
			copyBoard(&boardCopy);
			bool visited[13][13];
			memset(visited, false, sizeof(visited));
			visited[i][j] = true;
			debugPrint("X : " + std::to_string(i) + " : " + std::to_string(j) + " | " + std::to_string(moves->size()) + "\n");
			basicGenerator(moves, &boardCopy, i, j, &visited, Piece::maxSteps(piece), false, isWhite);
		}
	}

	return moves;
}

void BoardState::basicGenerator(std::shared_ptr<std::vector<std::vector<xMove>>> moves, uint8_t(*state)[13][13], int x, int y, bool(*visited)[13][13], int remainingSteps, bool turned, bool isWhite) const
{
	std::vector<BoardState::xMove> move = std::vector<BoardState::xMove>();
	move.reserve(10);
	for (int i = 0; i < 13; i++) {
		for (int j = 0; j < 13; j++) {
			if (pieces[i][j] == (*state)[i][j]) continue;
			move.emplace_back(xMove{ i, j, ((uint8_t)((pieces[i][j]) ^ ((*state)[i][j]))) });		//Creation of xor lists for moves
		}
	}
	if (moves->size() == 0 || move.size() > 0) {
		moves->emplace_back(move);
	}

	if (!turned && (((*state)[x][y] & hasTurnPiece) != 0)) {		//Turn in place if we can and haven't yet
		uint8_t boardCopy[13][13];
		std::memcpy(&boardCopy, state, sizeof(boardCopy));
		boardCopy[x][y] ^= setTurnPiece;
		bool visitedCopy[13][13];
		std::memcpy(&visitedCopy, visited, sizeof(visitedCopy));
		debugPrint("T : " + std::to_string(x) + " : " + std::to_string(y) + " | " + std::to_string(moves->size()) + "\n");
		basicGenerator(moves, &boardCopy, x, y, &visitedCopy, remainingSteps, true, isWhite);
	}

	if (remainingSteps == 0) return;


	/*
		0
	   3 1
		2
	*/
	for (int d = 0; d < 4; d++) {		//Loop through the 4 possible directions
		uint8_t piece = ((*state)[x][y]);
		if ((piece & hasTurnPiece) != 0) {		//Obey turn pieces
			if (((piece & setTurnPiece)) == (d % 2) * setTurnPiece) continue;		//This feels deeply cursed
		}
		int i = x;
		int j = y;
		switch (d)
		{
		case 0:
			j--;
			break;
		case 1:
			i++;
			break;
		case 2:
			j++;
			break;
		case 3:
			i--;
			break;
		}
		uint8_t dest = (*state)[i][j];
		if (i < 0 || i > 12 || j < 0 || j > 12) continue;
		if (i % 2 == 1 && j % 2 == 1) continue;
		if ((*visited)[i][j]) continue;		//Bounds and revisiting check
		if ((dest & hasTurnPiece) != 0 && (((dest & setTurnPiece)) == (d % 2) * setTurnPiece)) continue;

		if ((dest & 0b00111111) == 0) {		//Most basic case : empty target square
			for (int splitOff = 1; splitOff <= Piece::height(piece); splitOff++) {		//number of moved pieces
				uint8_t boardCopy[13][13];
				std::memcpy(&boardCopy, state, sizeof(boardCopy));
				if (splitOff == Piece::height(piece)) {
					boardCopy[x][y] &= 0b11000000;
				}
				else {
					boardCopy[x][y] -= splitOff;
					if (Piece::hasAddOn(piece) && splitOff == Piece::height(piece) - 1) {
						boardCopy[x][y] &= 0b11011111;		//erase trailing color
					}
				}
				boardCopy[i][j] |= (uint8_t)((piece) & 0b00100000 | splitOff);
				if (splitOff == Piece::height(piece)) {
					boardCopy[i][j] |= (uint8_t)((piece) & 0b00011000);		//addons, only if  we're moving the whole tower
				}
				bool visitedCopy[13][13];
				std::memcpy(&visitedCopy, visited, sizeof(visitedCopy));
				visitedCopy[i][j] = true;
				debugPrint("O : " + std::to_string(i) + " : " + std::to_string(j) + " | " + std::to_string(moves->size()) + "\n");
				basicGenerator(moves, &boardCopy, i, j, &visitedCopy, remainingSteps - 1, false, isWhite);
			}
		}
		else if (Piece::height(dest) > 0 && (Piece::isAddOn(dest) ? true : (Piece::colour(dest) == Piece::colour(piece)))) {		//merging
			for (int splitOff = 1; (splitOff <= Piece::height(piece)) && (splitOff <= 5 - Piece::height(dest)); splitOff++) {
				if (splitOff == Piece::height(piece) && Piece::hasAddOn(piece)) continue;
				uint8_t boardCopy[13][13];
				std::memcpy(&boardCopy, state, sizeof(boardCopy));
				if (splitOff == Piece::height(piece)) {
					boardCopy[x][y] &= 0b11000000;
				}
				else {
					boardCopy[x][y] -= splitOff;
					if (Piece::hasAddOn(piece) && splitOff == Piece::height(piece) - 1) {
						boardCopy[x][y] &= 0b11011111;		//erase trailing color
					}
				}
				boardCopy[i][j] |= (uint8_t)((piece) & 0b00100000);		//color info
				boardCopy[i][j] += splitOff;
				if (splitOff == Piece::height(piece)) {
					boardCopy[i][j] |= (uint8_t)((piece) & 0b00011000);		//addons, only if  we're moving the whole tower
				}
				bool visitedCopy[13][13];
				std::memcpy(&visitedCopy, visited, sizeof(visitedCopy));
				visitedCopy[i][j] = true;
				debugPrint("M : " + std::to_string(i) + " : " + std::to_string(j) + " | " + std::to_string(moves->size()) + "\n");
				basicGenerator(moves, &boardCopy, i, j, &visitedCopy, 0, false, isWhite);
			}
		}

		if (Piece::height(dest) > 0 && Piece::height(dest) <= Piece::height(piece) && (Piece::isAddOn(dest) ? false : (Piece::colour(dest) == Piece::colour(piece)))) {		//pushing (smaller than the pushing piece, not an addon and the same color)
			int offset = 2;
			bool pushValid = false;
			int pushHeight = Piece::height(dest);

			int xDir = 0;
			int yDir = 0;
			switch (d)
			{
			case 0:
				yDir = -1;
				break;
			case 1:
				xDir = 1;
				break;
			case 2:
				yDir = 1;
				break;
			case 3:
				xDir = -1;
				break;
			}

			/*
				Continue checking in the direction of movement until pushing is either posible or it isn't
				Offset is the offset from the original piece
				We loop until we hit a wall or something we can't push (hopefully)
			*/

			while (true) {
				int pushX = x + xDir * offset;
				int pushY = y + yDir * offset;

				if (pushX < 0 || pushX > 12 || pushY < 0 || pushY > 12) break;

				uint8_t pushPiece = ((*state)[pushX][pushY]);

				if ((pushPiece & hasTurnPiece) != 0) {		//Obey turn pieces
					if (((pushPiece & setTurnPiece)) == (d % 2) * setTurnPiece) break;		//This feels deeply cursed
				}
				if (Piece::height(pushPiece) == 0) {
					pushValid = true;
					break;
				}
				if (Piece::height(pushPiece) <= pushHeight && (Piece::isAddOn(pushPiece) ? false : (Piece::colour(pushPiece) == Piece::colour(piece)))) {
					offset++;
					pushHeight = Piece::height(pushPiece);
				}
				else { break; }
			}
			if (!pushValid) continue;
			uint8_t boardCopy[13][13];
			std::memcpy(&boardCopy, state, sizeof(boardCopy));

			for (int o = offset; o > 0; o--) {	//loop backwards from the end of the push chain and copy the pieces over
				int destX = x + xDir * o;
				int destY = y + yDir * o;
				int sourceX = x + xDir * (o - 1);
				int sourceY = y + yDir * (o - 1);

				boardCopy[destX][destY] |= boardCopy[sourceX][sourceY] & 0b00111111;
				boardCopy[sourceX][sourceY] &= 0b11000000;
			}

			bool visitedCopy[13][13];
			std::memcpy(&visitedCopy, visited, sizeof(visitedCopy));
			visitedCopy[i][j] = true;
			debugPrint("P : " + std::to_string(i) + " : " + std::to_string(j) + " | " + std::to_string(moves->size()) + "\n");
			basicGenerator(moves, &boardCopy, i, j, &visitedCopy, remainingSteps - 1, false, isWhite);
		}

		if (remainingSteps >= 2 && Piece::isTower(dest) && Piece::colour(dest) != Piece::colour(piece)) {		//capturing
																												//making sure we have enough moves left to be able to capture
			if (!Piece::isBlue(piece) && Piece::height(dest) > Piece::height(piece)) continue;		//capturing height check

			//Logic: figure out all ways to capture and then generate all splitting options at the end to not redo the work over and over again (this might be something good to cache when trying to optimize)

			uint8_t boardCopy[13][13];
			std::memcpy(&boardCopy, state, sizeof(boardCopy));
			boardCopy[i][j] &= 0b11000000; //remove piece we're capturing
			bool visitedCopy[13][13];
			std::memcpy(&visitedCopy, visited, sizeof(visitedCopy));
			visitedCopy[i][j] = true;
			debugPrint("C : " + std::to_string(i) + " : " + std::to_string(j) + " | " + std::to_string(moves->size()) + "\n");
			captureGenerator(moves, &boardCopy, x, y, i, j, &visitedCopy, remainingSteps - 1, false, isWhite);
		}
	}
}


void BoardState::captureGenerator(std::shared_ptr<std::vector<std::vector<xMove>>> moves, uint8_t(*state)[13][13], int originX, int originY, int x, int y, bool(*visited)[13][13], int remainingSteps, bool turned, bool isWhite) const
{
	/*
		0
	   3 1
		2
	*/
	uint8_t origin = ((*state)[originX][originY]);

	if (!turned && (((*state)[x][y] & hasTurnPiece) != 0)) {		//Turn in place if we can and haven't yet
		uint8_t boardCopy[13][13];
		std::memcpy(&boardCopy, state, sizeof(boardCopy));
		boardCopy[x][y] ^= setTurnPiece;
		bool visitedCopy[13][13];
		std::memcpy(&visitedCopy, visited, sizeof(visitedCopy));
		debugPrint("CT : " + std::to_string(x) + " : " + std::to_string(y) + " | " + std::to_string(moves->size()) + "\n");
		captureGenerator(moves, &boardCopy, originX, originY, x, y, &visitedCopy, remainingSteps, true, isWhite);
	}

	for (int d = 0; d < 4; d++) {		//Loop through the 4 possible directions
		uint8_t piece = ((*state)[x][y]);
		if ((piece & hasTurnPiece) != 0) {		//Obey turn pieces
			if (((piece & setTurnPiece)) == (d % 2) * setTurnPiece) continue;		//This feels deeply cursed
		}
		int i = x;
		int j = y;
		switch (d)
		{
		case 0:
			j--;
			break;
		case 1:
			i++;
			break;
		case 2:
			j++;
			break;
		case 3:
			i--;
			break;
		}
		uint8_t dest = (*state)[i][j];
		if (i < 0 || i > 12 || j < 0 || j > 12) continue;
		if (i % 2 == 1 && j % 2 == 1) continue;
		if ((*visited)[i][j]) continue;		//Bounds and revisiting check
		if ((dest & hasTurnPiece) != 0 && (((dest & setTurnPiece)) == (d % 2) * setTurnPiece)) continue;

		//piece is the position where an enemy piece used to be that we are currently taking, this value has already been wiped
		//dest is the piece we are now looking to move to, either to complete the capture or to continue the chain
		//the original piece doing the capturing still exists at originX, originY in the state array
		//splitting will be done once the end of a capture chain has been found


		//ending the chain on an empty square or a friendly tower or a single addon to merge onto
		if (Piece::height(dest) == 0 || (Piece::isTower(dest) && Piece::colour(dest) == Piece::colour(origin)) || Piece::isAddOn(dest)) {

			//We have to split less than the full tower, except if the height is 1 and we can't go over the merge limit
			for (int splitOff = 1; (Piece::height(origin) == 1 ? splitOff <= 1 : (splitOff < Piece::height(origin))) && (splitOff <= 5 - Piece::height(dest)); splitOff++) {

				uint8_t boardCopy[13][13];
				std::memcpy(&boardCopy, state, sizeof(boardCopy));
				if (Piece::height(origin) == 1) {
					boardCopy[originX][originY] &= 0b11000000;		//wipe the orign only if the height is one
				}
				else {
					boardCopy[originX][originY] -= splitOff;
					if (Piece::isAddOn(boardCopy[originX][originY])) boardCopy[originX][originY] &= 0b11011111;		//if we left a single addon, remove the color
				}

				boardCopy[i][j] += splitOff;
				boardCopy[i][j] |= Piece::colour(origin);		//always copy color and add height as that should be allowed based on the only way this can be called

				bool visitedCopy[13][13];
				std::memcpy(&visitedCopy, visited, sizeof(visitedCopy));
				visitedCopy[i][j] = true;
				debugPrint("CM : " + std::to_string(i) + " : " + std::to_string(j) + " | " + std::to_string(moves->size()) + "\n");
				basicGenerator(moves, &boardCopy, i, j, &visitedCopy, 0, false, isWhite);		//do last move
				//TODO: Wrong ruleset bug, verify this change is correct
				// //if dest is empty, continue the move if possible
				//basicGenerator(moves, &boardCopy, i, j, &visitedCopy, Piece::height(dest) == 0 ? remainingSteps - 1 : 0, false, isWhite); 
			}
		}
		else if (remainingSteps + Piece::isBlue(origin) > 1 && Piece::isTower(dest) && Piece::colour(origin) != Piece::colour(dest)) {
			if (!Piece::isBlue(origin) && Piece::height(dest) > Piece::height(origin)) continue;		//capturing height check
			uint8_t boardCopy[13][13];
			std::memcpy(&boardCopy, state, sizeof(boardCopy));
			boardCopy[i][j] &= 0b11000000; //remove piece we're capturing
			bool visitedCopy[13][13];
			std::memcpy(&visitedCopy, visited, sizeof(visitedCopy));
			visitedCopy[i][j] = true;
			debugPrint("CC : " + std::to_string(i) + " : " + std::to_string(j) + " | " + std::to_string(moves->size()) + "\n");
			captureGenerator(moves, &boardCopy, originX, originY, i, j, &visitedCopy, remainingSteps - !Piece::isBlue(origin), false, isWhite);		//only decrement moves if the capturing piece doesn't have a blue addon
		}
	}
}

void BoardState::unsafeMakeMove(std::vector<xMove>* move)
{
	for (int i = 0; i < move->size(); i++) {
		pieces[(*move)[i].i][(*move)[i].j] ^= (*move)[i].delta;
	}
}

int BoardState::makeMove(std::vector<xMove>* move, bool isWhiteTurn)
{
	std::shared_ptr<std::vector<std::vector<BoardState::xMove>>> moves;
	moves = getMoves(isWhiteTurn);
	uint8_t boardCopy1[13][13];
	uint8_t boardCopy2[13][13];
	std::memcpy(&boardCopy1, pieces, sizeof(boardCopy1));
	for (int i = 0; i < move->size(); i++) {
		boardCopy1[(*move)[i].i][(*move)[i].j] ^= (*move)[i].delta;
	}
	for (int i = 0; i < moves->size(); i++) {
		std::memcpy(&boardCopy2, pieces, sizeof(boardCopy2));
		for (int j = 0; j < (*moves)[i].size(); j++) {
			boardCopy2[(*moves)[i][j].i][(*moves)[i][j].j] ^= (*moves)[i][j].delta;
		}
		if (memcmp(&boardCopy1, &boardCopy2, sizeof(boardCopy1)) == 0) {
			std::memcpy(&pieces, boardCopy1, sizeof(pieces));
			return i == 0 ? 0 : 1;
		}
	}
	return -1;
}

int BoardState::makeMove(uint8_t(*newState)[13][13], bool isWhiteTurn)
{
	std::shared_ptr<std::vector<std::vector<BoardState::xMove>>> moves;
	moves = getMoves(isWhiteTurn);
	uint8_t boardCopy[13][13];
	for (int i = 0; i < moves->size(); i++) {
		std::memcpy(&boardCopy, pieces, sizeof(boardCopy));
		for (int j = 0; j < (*moves)[i].size(); j++) {
			boardCopy[(*moves)[i][j].i][(*moves)[i][j].j] ^= (*moves)[i][j].delta;
		}
		if (memcmp(newState, &boardCopy, sizeof(*newState)) == 0) {
			std::memcpy(&pieces, newState, sizeof(pieces));

			forceDebugPrint(dumpPos());
			forceDebugPrint("\n");

			gameRecord.emplace_back(std::to_string(i) + " ");
			return i == 0 ? 0 : 1;
		}
	}
	return -1;
}
BoardState::winValue BoardState::gameOver(bool isWhite) //last player to make a move
{
	//Check for full eliminations, if yes, that player wins
	//Check for base wins
	//Check for port wins
	//Check to see if a player has one and make sure the right one wins, incuding the Bridge Rule

	bool whiteNoPieces = true;
	bool blackNoPieces = true;
	for (int i = 0; i < 13 && (whiteNoPieces || blackNoPieces); i++) {
		for (int j = 0; j < 13 && (whiteNoPieces || blackNoPieces); j++) {
			if (Piece::isTower(pieces[i][j])) {
				if (Piece::isWhite(pieces[i][j])) {
					whiteNoPieces = false;
				}
				else {
					blackNoPieces = false;
				}
			}
		}
	}
	if (whiteNoPieces) return winValue::black;
	if (blackNoPieces) return winValue::white;
	//There can be no draw if one side has no pieces left so we don't need any extra checks

	bool bases[2][2] = { {false, false}, {false, false} }; //[0] white   [1] black   [x][0] white base   [x][1] black base
	for (int i = 5; i < 8; i++) {
		for (int j = 0; j < 4; j++) {
			if (Piece::isTower(pieces[i][j])) {
				if (Piece::isWhite(pieces[i][j])) {
					bases[0][1] = true;	//black base white piece
				}
				else {
					bases[1][1] = true;	//black base black piece
				}
			}

			if (Piece::isTower(pieces[i][12 - j])) {
				if (Piece::isWhite(pieces[i][12 - j])) {
					bases[0][0] = true;	//white base white piece
				}
				else {
					bases[1][0] = true;	//white base black piece
				}
			}
		}
	}

	bool whiteWin = false;
	bool blackWin = false;
	if (bases[0][1] && !bases[1][1]) whiteWin = true;
	if (bases[1][0] && !bases[0][0]) blackWin = true;

	bool portControll[4][2] = { false, false, false, false , false, false, false, false }; //[top left, top right, bottom left, bottom right],[white, black]
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			for (int corner = 0; corner < 4; corner++) {
				uint8_t piece;
				switch (corner) {
				case 0:
					piece = pieces[i][j];
					break;
				case 1:
					piece = pieces[12 - i][j];
					break;
				case 2:
					piece = pieces[i][12 - j];
					break;
				case 3:
					piece = pieces[12 - i][12 - j];
					break;
				}
				if (Piece::isTower(piece)) {
					portControll[corner][1 - Piece::isWhiteTower(piece)] = true;
				}
			}

		}
	}
	int portNumbers[2] = { 0, 0 };
	for (int i = 0; i < 4; i++) {
		portNumbers[0] += portControll[i][0];
		portNumbers[1] += portControll[i][1];
	}
	if (portNumbers[0] == 4 && portNumbers[1] == 0) whiteWin = true;
	if (portNumbers[1] == 4 && portNumbers[0] == 0) blackWin = true;
	if (!(whiteWin || blackWin)) {
		return winValue::none;
	}
	if (whiteWin && !blackWin) return winValue::white;
	if (!whiteWin && blackWin) return winValue::black;
	return isWhite ? winValue::white : winValue::black;
}

std::string BoardState::dumpPos()
{
	std::string turnPieceStr = "";
	std::string str = "";

	for (int i = 0; i < 13; i++) {
		for (int j = 0; j < 13; j++) {
			uint8_t piece = pieces[i][j];

			if (!((piece & hasTurnPiece) == 0)) {
				turnPieceStr += ((piece & setTurnPiece) == 0) ? "0" : "1";
			}

			if (Piece::height(piece) == 0) continue;
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

void BoardState::loadPos(std::string str)
{
	wipe();

	std::string delimiter = " ";

	size_t pos = 0;
	std::string token;
	while ((pos = str.find(delimiter)) != std::string::npos) {
		token = str.substr(0, pos);
		uint8_t piece = 0;
		if (token[0] == 'b') piece |= Piece::Blue;
		if (token[0] == 'r') piece |= Piece::Red;
		if (token[1] == 'B') piece |= Piece::Black;
		piece += token[2] - '0';
		int x = 10 * (token[3] - '0') + (token[4] - '0');
		int y = 10 * (token[5] - '0') + (token[6] - '0');
		pieces[x][y] |= piece;

		str.erase(0, pos + delimiter.length());
	}
	token = str;
	int n = 0;
	for (int i = 0; i < 13; i++) {
		for (int j = 0; j < 13; j++) {
			if ((pieces[i][j] & hasTurnPiece) == 0) continue;
			pieces[i][j] |= (token[n] == '1') ? setTurnPiece : 0;
			n++;
		}
	}
}

void BoardState::dumpGame()
{

	forceDebugPrint("\nGame record:\n\n");
	for (int i = 0; i < gameRecord.size(); i++) {
		forceDebugPrint(gameRecord[i]);
	}
	forceDebugPrint("\n\n");
}
