#include <cmath>
#include <memory>
#include <vector>
#include <string>
#include <Windows.h>
#include <WinUser.h>
#include <bitset>

#include "Utils.h"
#include "../Globals/Piece.h"
#include "../Test Bots/BasicGenerator.h"

void Utils::print(std::string str) {
	std::wstring temp = std::wstring(str.begin(), str.end());
	LPCWSTR wideString = temp.c_str();
	OutputDebugString(wideString);
}

void Utils::print(float f) {
	std::string str = std::to_string(f);
	std::wstring temp = std::wstring(str.begin(), str.end());
	LPCWSTR wideString = temp.c_str();
	OutputDebugString(wideString);
}

void Utils::print(int f) {
	std::string str = std::to_string(f);
	std::wstring temp = std::wstring(str.begin(), str.end());
	LPCWSTR wideString = temp.c_str();
	OutputDebugString(wideString);
}

void Utils::print(BasicGenerator::xMove f) {
	std::string str = std::to_string(f.i);
	str += " | ";
	str += std::to_string(f.j);
	str += " | ";
	std::bitset<8> x(f.delta);
	str += x.to_string();
	str += "\n";
	std::wstring temp = std::wstring(str.begin(), str.end());
	LPCWSTR wideString = temp.c_str();
	OutputDebugString(wideString);
}

Utils::winValue Utils::gameOver(bool isWhite, uint8_t(*pieces)[13][13]) //last player to make a move
{
	//Check for full eliminations, if yes, that player wins
	//Check for base wins
	//Check for port wins
	//Check to see if a player has one and make sure the right one wins, incuding the Bridge Rule

	bool whiteNoPieces = true;
	bool blackNoPieces = true;
	for (int i = 0; i < 13 && (whiteNoPieces || blackNoPieces); i++) {
		for (int j = 0; j < 13 && (whiteNoPieces || blackNoPieces); j++) {
			if (Piece::isTower((*pieces)[i][j])) {
				if (Piece::isWhite((*pieces)[i][j])) {
					whiteNoPieces = false;
				}
				else {
					blackNoPieces = false;
				}
			}
		}
	}
	if (whiteNoPieces) return winValue::black;
	if (whiteNoPieces) return winValue::white;
	//There can be no draw if one side has no pieces left so we don't need any extra checks

	bool bases[2][2] = { {false, false}, {false, false} }; //[0] white   [1] black   [x][0] white base   [x][1] black base
	for (int i = 5; i < 8; i++) {
		for (int j = 0; j < 4; j++) {
			if (Piece::isTower((*pieces)[i][j])) {
				if (Piece::isWhite((*pieces)[i][j])) {
					bases[0][1] = true;	//black base white piece
				}
				else {
					bases[1][1] = true;	//black base black piece
				}
			}

			if (Piece::isTower((*pieces)[i][12 - j])) {
				if (Piece::isWhite((*pieces)[i][12 - j])) {
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
					piece = (*pieces)[i][j];
					break;
				case 1:
					piece = (*pieces)[12 - i][j];
					break;
				case 2:
					piece = (*pieces)[i][12 - j];
					break;
				case 3:
					piece = (*pieces)[12 - i][12 - j];
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

float Utils::basicPosEval(bool isWhite, uint8_t(*pieces)[13][13])
{
	winValue win = gameOver(isWhite, pieces);
	if (win == winValue::black) return -9999;
	if (win == winValue::white) return 9999;

	float eval = 0;
	for (int i = 0; i < 13; i++) {
		for (int j = 0; j < 13; j++) {
			uint8_t piece = (*pieces)[i][j];
			if (!Piece::isTower(piece)) continue;
			eval += (Piece::height(piece) + (Piece::height(piece)==1?-0.6:0) + 2 * (Piece::hasAddOn(piece)));
			eval *= (Piece::isWhiteTower(piece) ? 1 : -1);
		}
	}
	return eval;
}

int Utils::getBestMoveBasic(bool isWhite, uint8_t(*pieces)[13][13])
{
	std::shared_ptr<std::vector<std::vector<BasicGenerator::xMove>>> moves;
	moves = BasicGenerator::getMoves(isWhite, pieces);

	std::vector<int> bestMoves;
	float bestEval = -99999;

	for (int j = 1; j < moves->size(); j++) {
		//if (((*moves)[j]).size() == 0) continue;
		uint8_t boardCopy[13][13];
		std::memcpy(&boardCopy, pieces, sizeof(boardCopy));
		for (int i = 0; i < ((*moves)[j]).size(); i++) {
			boardCopy[((*moves)[j])[i].i][((*moves)[j])[i].j] ^= ((*moves)[j])[i].delta;
		}
		float posEval = basicPosEval(isWhite, &boardCopy) * (isWhite?1:-1);
		//printU(std::to_string(bestEval));
		//printU(" : ");
		//printU(std::to_string(posEval));
		
		if (posEval > (bestEval+0.01)) {
			//printU(" +");
			bestMoves.clear();
			bestMoves.emplace_back(j);
			bestEval = posEval;
		}
		else if (posEval >= bestEval-0.005) {
			bestMoves.emplace_back(j);
			//printU(" =");
		}
		//printU("\n");
	}

	int random = rand() % bestMoves.size();
	//print(bestMoves[random]);
	//print("\n");
	return bestMoves[random];
}
