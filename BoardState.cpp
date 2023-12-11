#include "BoardState.h"
#include <iostream>
#include <string>
#include "Piece.h"
#include <cstdint>

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

	pieces[0][0] = turnPiece;
	pieces[2][0] = turnPiece;
	pieces[0][2] = turnPiece;
	pieces[2][2] = turnPiece;

	pieces[10][0] = turnPiece;
	pieces[12][0] = turnPiece;
	pieces[10][2] = turnPiece;
	pieces[12][2] = turnPiece;

	pieces[0][12] = turnPiece;
	pieces[2][12] = turnPiece;
	pieces[0][10] = turnPiece;
	pieces[2][10] = turnPiece;

	pieces[10][12] = turnPiece;
	pieces[12][12] = turnPiece;
	pieces[10][10] = turnPiece;
	pieces[12][10] = turnPiece;



	pieces[6][0] = turnPiece;
	pieces[6][2] = turnPiece;
	pieces[6][10] = turnPiece;
	pieces[6][12] = turnPiece;



	pieces[2][6] = turnPiece;
	pieces[4][6] = turnPiece;
	pieces[8][6] = turnPiece;
	pieces[10][6] = turnPiece;
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



	pieces[2][6] &= turnPieceMask & pieces[2][6] | (tps[2] * setTurnPiece);
	pieces[4][6] &= turnPieceMask & pieces[4][6] | (tps[2] * setTurnPiece);
	pieces[8][6] &= turnPieceMask & pieces[8][6] | (tps[2] * setTurnPiece);
	pieces[10][6] &= turnPieceMask & pieces[10][6] | (tps[2] * setTurnPiece);

	for (int i = 0; i < 13; i++) {
		for (int j = 0; j < 13; j++) {
			pieces[i][j] &= resetMask;
		}
	}

	/*pieces[5][0] |= 3 | Piece::Blue;
	pieces[7][0] |= 3 | Piece::Red;
	pieces[5][2] |= 3;
	pieces[7][2] |= 3;*/


	/*pieces[2][4] |= 2 | Piece::Blue;
	pieces[4][4] |= 1;
	pieces[6][4] |= 2;
	pieces[2][5] |= 1 | Piece::Black;
	pieces[2][3] |= 1 | Piece::Red;

	pieces[5][10] |= 3 | Piece::Black | Piece::Blue;
	pieces[7][10] |= 3 | Piece::Black | Piece::Red;
	pieces[5][12] |= 5 | Piece::Black;
	pieces[7][12] |= 1 | Piece::Black;

	pieces[0][0] |= Piece::Blue | 1;
	pieces[12][0] |= Piece::Blue | 1;
	pieces[0][12] |= Piece::Blue | 1;
	pieces[12][12] |= Piece::Blue | 1;
	pieces[6][6] |= Piece::Blue | 1;

	pieces[0][6] |= Piece::Red | 1;
	pieces[12][6] |= Piece::Red | 1;*/

	pieces[5][10] = Piece::White | 1;
	pieces[6][9] = Piece::White | 5;
	pieces[2][8] = Piece::White | 1;
	pieces[5][4] = Piece::White | 2 | Piece::Blue;
	pieces[8][4] = Piece::White | 1;
	pieces[6][3] = Piece::White | 1;
	pieces[8][3] = Piece::White | 3 | Piece::Red;
	pieces[6][8] = Piece::Black | 2 | Piece::Blue;
	pieces[6][6] = Piece::Black | 4 | Piece::Red;
	pieces[3][2] = Piece::Black | 2 | Piece::Blue;
	pieces[6][2] = Piece::Black | 3;
}

std::string center(uint8_t piece) {
	std::string out = BACKGROUND;
	if ((piece & BoardState::turnPiece) != 0) { out += (((piece & BoardState::setTurnPiece) == 0) ? std::string(1, (unsigned char)205) : std::string(1, (unsigned char)186)); }
	else { out += " "; }

	//if (Piece::isWhiteTower(piece)) { out += WHITE; }
	if (Piece::height(piece) > 0) {
		out += Piece::isWhite(piece) ? WHITE : BLACK;
	}
	out += Piece::height(piece) == 0 ? " " : std::to_string(Piece::height(piece));
	out += (Piece::isBlue(piece) ? BLUE : Piece::isRed(piece) ? RED : " ");
	out += BACKGROUND;

	if ((piece & BoardState::turnPiece) != 0) { out += (((piece & BoardState::setTurnPiece) == 0) ? std::string(1, (unsigned char)205) : std::string(1, (unsigned char)186)); }
	else { out += " "; }
	return out;
};

void BoardState::copyBoard(uint8_t(*dest)[13][13])
{
	std::memcpy(dest, &pieces, sizeof(pieces));
}

struct BoardState::xMove {
	int i;
	int j;
	uint8_t delta;
	xMove(int I, int J, uint8_t D) : i(I), j(J), delta(D) {}
};

void debugPrint(std::string str) {
	if (!DEBUG_PRINTING) return;
	std::wstring temp = std::wstring(str.begin(), str.end());
	LPCWSTR wideString = temp.c_str();
	OutputDebugString(wideString);
}

std::shared_ptr<std::vector<std::vector<BoardState::xMove>>> BoardState::getMoves(bool isWhite) {
	std::shared_ptr<std::vector<std::vector<xMove>>> moves = std::make_shared<std::vector<std::vector<xMove>>>();

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
			debugPrint("X : " + std::to_string(i) + " : " + std::to_string(j) + "\n");
			basicGenerator(moves, &boardCopy, i, j, &visited, Piece::maxSteps(piece), false, isWhite);
		}
	}

	return moves;
}

void BoardState::basicGenerator(std::shared_ptr<std::vector<std::vector<xMove>>> moves, uint8_t(*state)[13][13], int x, int y, bool(*visited)[13][13], int remainingSteps, bool turned, bool isWhite)
{
	std::vector<BoardState::xMove> move = std::vector<BoardState::xMove>();
	for (int i = 0; i < 13; i++) {
		for (int j = 0; j < 13; j++) {
			if (pieces[i][j] == (*state)[i][j]) continue;
			move.emplace_back(xMove{ i, j, ((uint8_t)((pieces[i][j]) ^ ((*state)[i][j]))) });		//Creation of xor lists for moves
		}
	}
	moves->emplace_back(move);

	if (!turned && (((*state)[x][y] & turnPiece) != 0)) {		//Turn in place if we can and haven't yet
		uint8_t boardCopy[13][13];
		std::memcpy(&boardCopy, state, sizeof(boardCopy));
		boardCopy[x][y] ^= setTurnPiece;
		bool visitedCopy[13][13];
		std::memcpy(&visitedCopy, visited, sizeof(visitedCopy));
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
		if ((piece & turnPiece) != 0) {		//Obey turn pieces
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
		if ((dest & turnPiece) != 0 && (((dest & setTurnPiece)) == (d % 2) * setTurnPiece)) continue;

		if ((dest & 0b00111111) == 0) {		//Most basic case : empty target square
			for (int splitOff = 1; splitOff <= Piece::height(piece); splitOff++) {
				uint8_t boardCopy[13][13];
				std::memcpy(&boardCopy, state, sizeof(boardCopy));
				if (splitOff == Piece::height(piece)) {
					boardCopy[x][y] &= 0b11000000;
				}
				else {
					boardCopy[x][y] -= splitOff;
					if (splitOff == Piece::height(piece) - 1) {
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
				debugPrint("O : " + std::to_string(i) + " : " + std::to_string(j) + "\n");
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
					if (splitOff == Piece::height(piece) - 1) {
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
				debugPrint("M : " + std::to_string(i) + " : " + std::to_string(j) + "\n");
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

				if ((pushPiece & turnPiece) != 0) {		//Obey turn pieces
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
			debugPrint("P : " + std::to_string(i) + " : " + std::to_string(j) + "\n");
			basicGenerator(moves, &boardCopy, i, j, &visitedCopy, remainingSteps - 1, false, isWhite);
		}
	}
}

void BoardState::captureGenerator(uint8_t(*state)[13][13], int x, int y, bool(*visited)[13][13], int remainingSteps, bool blue, bool isWhite)
{

}

void BoardState::unsafeMakeMove(std::vector<xMove>* move)
{
	for (int i = 0; i < move->size(); i++) {
		pieces[(*move)[i].i][(*move)[i].j] ^= (*move)[i].delta;
	}
}

void BoardState::makeMove(std::vector<xMove>* move, bool isWhiteTurn)
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
			return;
		}
	}
}
