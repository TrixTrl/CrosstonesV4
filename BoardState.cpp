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
	pieces[2][2] |= 3;

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
	pieces[12][6] |= Piece::Red | 1;
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

void BoardState::print()
{
	auto s = [](int i)->std::string {
		return std::string(1, (unsigned char)i);
	};

	std::string basicTop = BACKGROUND + s(218) + s(196) + s(196) + s(196) + s(196) + s(191) + RESET;
	std::string basicBottom = BACKGROUND + s(192) + s(196) + s(196) + s(196) + s(196) + s(217) + RESET;
	std::string basicSide = BACKGROUND + s(179) + RESET;
	std::string turnTop = BACKGROUND + s(201) + s(205) + s(205) + s(205) + s(205) + s(187) + RESET;
	std::string turnBottom = BACKGROUND + s(200) + s(205) + s(205) + s(205) + s(205) + s(188) + RESET;
	std::string turnSide = BACKGROUND + s(186) + RESET;

	std::cout << "    1     2     3     4     5     6     7     8     9    10    11    12    13  \n";

	for (int i = 0; i < 13; i++) {
		for (int l = 0; l < 3; l++) {
			if (l == 1) {
				std::cout << s(77 - i);
			}
			else {
				std::cout << " ";
			}
			for (int j = 0; j < 13; j++) {
				uint8_t piece = pieces[j][i];
				if (i % 2 == 1 && j % 2 == 1) {
					std::cout << "      ";
				}
				else {
					switch (l) {
					case 0:
						if ((piece & turnPiece) != 0) {
							std::cout << turnTop;
						}
						else std::cout << basicTop;
						break;
					case 1:
						if ((piece & turnPiece) != 0) {
							std::cout << turnSide << center(piece) << turnSide;
						}
						else std::cout << basicSide << center(piece) << basicSide;
						break;
					case 2:
						if ((piece & turnPiece) != 0) {
							std::cout << turnBottom;
						}
						else std::cout << basicBottom;
						break;
					}
				}
			}
			if (l == 1) std::cout << s(77 - i);
			std::cout << "\n";
		}
	}
	std::cout << "    1     2     3     4     5     6     7     8     9    10    11    12    13  \n";
}

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
	std::wstring temp = std::wstring(str.begin(), str.end());
	LPCWSTR wideString = temp.c_str();
	OutputDebugString(wideString);
}

std::vector<std::vector<BoardState::xMove>>* BoardState::getMoves(bool isWhite)
{
	std::vector<std::vector<xMove>>* moves = new std::vector<std::vector<xMove>>;		//HEAP ALLOCATED!!! HAS TO BE DECONSTRUCTED OR MEMORY WILL BE FILLED UP

	for (int i = 0; i < 13; i++) {
		for (int j = 0; j < 13; j++) {
			uint8_t piece = pieces[i][j];
			if (Piece::height(piece) == 0 || Piece::isAddOn(piece)) continue;
			if (Piece::isWhiteTower(piece) == isWhite) continue;

			uint8_t boardCopy[13][13];
			copyBoard(&boardCopy);
			bool visited[13][13];
			memset(visited, false, sizeof(visited));
			visited[i][j] = true;
			debugPrint("X : " + std::to_string(i) + " : " + std::to_string(j) + "\n");
			basicGenerator(moves, &boardCopy, i, j, &visited, Piece::maxSteps(piece), false);
		}
	}

	return moves;
}

void BoardState::basicGenerator(std::vector<std::vector<xMove>>* moves, uint8_t(*state)[13][13], int x, int y, bool(*visited)[13][13], int remainingSteps, bool turned)
{
	std::vector<BoardState::xMove>* move = new std::vector<BoardState::xMove>;
	for (int i = 0; i < 13; i++) {
		for (int j = 0; j < 13; j++) {
			if (pieces[i][j] == (*state)[i][j]) continue;
			move->emplace_back(xMove{ i, j, ((uint8_t)((pieces[i][j]) ^ ((*state)[i][j]))) });		//Creation of xor lists for moves
		}
	}
	moves->emplace_back(*move);

	if (!turned && (((*state)[x][y] & turnPiece) != 0)) {		//Turn in place if we can and haven't yet
		uint8_t boardCopy[13][13];
		std::memcpy(&boardCopy, state, sizeof(boardCopy));
		boardCopy[x][y] ^= setTurnPiece;
		bool visitedCopy[13][13];
		std::memcpy(&visitedCopy, visited, sizeof(visitedCopy));
		basicGenerator(moves, &boardCopy, x, y, &visitedCopy, remainingSteps, true);
	}

	if (remainingSteps == 0) return;


	/*
		0
	   3 1
		2
	*/
	for (int d = 0; d < 4; d++) {		//Loop through the 4 possible directions
		if ((((*state)[x][y]) & turnPiece) != 0) {		//Obey turn pieces
			if (((((*state)[x][y]) & setTurnPiece)) == (d % 2) * setTurnPiece) continue;		//This feels deeply cursed
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
		if (i < 0 || i > 12 || j < 0 || j > 12) continue;
		if (i % 2 == 1 && j % 2 == 1) continue;
		if ((*visited)[i][j]) continue;		//Bounds and revisiting check
		if ((((*state)[i][j]) & turnPiece) != 0 && (((((*state)[i][j]) & setTurnPiece)) == (d % 2) * setTurnPiece)) continue;

		if (((*state)[i][j] & 0b00111111) == 0) {		//Most basic case : empty target square
			uint8_t boardCopy[13][13];
			std::memcpy(&boardCopy, state, sizeof(boardCopy));
			boardCopy[x][y] &= 0b11000000;
			boardCopy[i][j] |= (uint8_t)(((*state)[x][y]) & 0b00111111);
			bool visitedCopy[13][13];
			std::memcpy(&visitedCopy, visited, sizeof(visitedCopy));
			visitedCopy[i][j] = true;
			debugPrint("O : " + std::to_string(i) + " : " + std::to_string(j) + "\n");
			basicGenerator(moves, &boardCopy, i, j, &visitedCopy, remainingSteps - 1, false);
		}
	}
}

void BoardState::unsafeMakeMove(std::vector<xMove>* move)
{
	for (int i = 0; i < move->size(); i++) {
		pieces[(*move)[i].i][(*move)[i].j] ^= (*move)[i].delta;
	}
}
