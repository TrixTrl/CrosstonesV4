#include <cmath>
#include <memory>
#include <vector>
#include <string>
#include <Windows.h>
#include <WinUser.h>
#include <bitset>
#include <limits>
#include <algorithm>
#include <thread>

#include "Utils.h"
#include "../Globals/Piece.h"
#include "../Test Bots/BasicGenerator.h"

#define MAXEVAL 9999
#define BITSETINDEX (i+(j*13))

void Utils::print(std::string str, bool newLine) {
	if (newLine) str += "\n";
	std::wstring temp = std::wstring(str.begin(), str.end());
	LPCWSTR wideString = temp.c_str();
	OutputDebugString(wideString);
}

void Utils::print(float f, bool newLine) {
	std::string str = std::to_string(f);
	if (newLine) str += "\n";
	std::wstring temp = std::wstring(str.begin(), str.end());
	LPCWSTR wideString = temp.c_str();
	OutputDebugString(wideString);
}

void Utils::print(int f, bool newLine) {
	std::string str = std::to_string(f);
	if (newLine) str += "\n";
	std::wstring temp = std::wstring(str.begin(), str.end());
	LPCWSTR wideString = temp.c_str();
	OutputDebugString(wideString);
}

void Utils::print(size_t f, bool newLine) {
	std::string str = std::to_string(f);
	if (newLine) str += "\n";
	std::wstring temp = std::wstring(str.begin(), str.end());
	LPCWSTR wideString = temp.c_str();
	OutputDebugString(wideString);
}

void Utils::print(BasicGenerator::xMove f, bool newLine) {
	std::string str = std::to_string(f.i);
	str += " | ";
	str += std::to_string(f.j);
	str += " | ";
	std::bitset<8> x(f.delta);
	str += x.to_string();
	if (newLine) str += "\n";
	std::wstring temp = std::wstring(str.begin(), str.end());
	LPCWSTR wideString = temp.c_str();
	OutputDebugString(wideString);
}

void Utils::print(extraBoardData data, bool formatted)
{
	std::string str;
	if (formatted) {
		str = "White:           Black:\n";

		std::string board[13];
		for (int i = 0; i < 2; i++) {

			for (int s = 0; s < 13; s++) {
				board[s] += (i == 0) ? "   " : "    ";
			}

			int X = 0;
			int Y = 0;
			for (int j = 0; j < 3; j++) {
				std::bitset<64> x(data.colorBitboards[i][j]);

				//str += "   ";
				//str += x.to_string();
				//str += "\n";

				for (int k = 0; k < 64 && Y < 13; k++) {
					board[Y] += x[k] ? "1" : "0";
					X++;
					if (X >= 13) {
						X = 0;
						Y++;
					}
				}
			}

		}

		for (int k = 0; k < 13; k++) {
			str += board[k] + "\n";
		}
	}
	else {
		for (int i = 0; i < 2; i++) {
			str += (i == 0) ? "White:\n" : "Black:\n";
			for (int j = 0; j < 3; j++) {
				std::bitset<64> x(data.colorBitboards[i][j]);

				str += "   ";
				str += x.to_string();
				str += "\n";
			}
		}
	}

	std::wstring temp = std::wstring(str.begin(), str.end());
	LPCWSTR wideString = temp.c_str();
	OutputDebugString(wideString);
}

Utils::winValue Utils::gameOver(bool isWhite, extraBoardData metaData)
{
	bool whiteNoPieces = !(metaData.colorBitboards[0][0] | metaData.colorBitboards[0][1] | metaData.colorBitboards[0][2]);
	bool blackNoPieces = !(metaData.colorBitboards[1][0] | metaData.colorBitboards[1][1] | metaData.colorBitboards[1][2]);

	if (whiteNoPieces) return winValue::black;
	if (blackNoPieces) return winValue::white;

	//[0] white   [1] black   [x][0] white base   [x][1] black base

	bool bases[2][2] = { {(metaData.colorBitboards[0][1] & 0b0001110000000000000000000000000000000000000000000000000000000000) | (metaData.colorBitboards[0][2] & 0b0000000000000000000000000000111000000000011100000000001110000000),
						  (metaData.colorBitboards[0][0] & 0b0000000000000000011100000000001110000000000111000000000011100000)},
						 {(metaData.colorBitboards[1][1] & 0b0001110000000000000000000000000000000000000000000000000000000000) | (metaData.colorBitboards[1][2] & 0b0000000000000000000000000000111000000000011100000000001110000000),
						  (metaData.colorBitboards[1][0] & 0b0000000000000000011100000000001110000000000111000000000011100000)}
	};

	bool whiteWin = false;
	bool blackWin = false;
	if (bases[0][1] && !bases[1][1]) whiteWin = true;
	if (bases[1][0] && !bases[0][0]) blackWin = true;

	//[top left, top right, bottom left, bottom right],[white, black]
	bool portControll[4][2] = { {(metaData.colorBitboards[0][0] & 0b0000000000000000000001111000000000111100000000011110000000001111), (metaData.colorBitboards[1][0] & 0b0000000000000000000001111000000000111100000000011110000000001111)},

								{(metaData.colorBitboards[0][0] & 0b0000000000001111000000000111100000000011110000000001111000000000), (metaData.colorBitboards[1][0] & 0b0000000000001111000000000111100000000011110000000001111000000000)},

								{(metaData.colorBitboards[0][1] & 0b0000000111100000000000000000000000000000000000000000000000000000) | (metaData.colorBitboards[0][2] & 0b0000000000000000000000000000000011110000000001111000000000111100),
								 (metaData.colorBitboards[1][1] & 0b0000000111100000000000000000000000000000000000000000000000000000) | (metaData.colorBitboards[1][2] & 0b0000000000000000000000000000000011110000000001111000000000111100)},

								{(metaData.colorBitboards[0][1] & 0b1100000000000000000000000000000000000000000000000000000000000000) | (metaData.colorBitboards[0][2] & 0b0000000000000000000000011110000000001111000000000111100000000011),
								 (metaData.colorBitboards[1][1] & 0b1100000000000000000000000000000000000000000000000000000000000000) | (metaData.colorBitboards[1][2] & 0b0000000000000000000000011110000000001111000000000111100000000011)} };

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
				if (Piece::isWhiteTower((*pieces)[i][j])) {
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
	extraBoardData data = generateMetadata(pieces);
	winValue win = gameOver(isWhite, data);
	if (win == winValue::black) return -9999;
	if (win == winValue::white) return 9999;

	float eval = 0;
	for (int i = 0; i < 13; i++) {
		for (int j = 0; j < 13; j++) {
			uint8_t piece = (*pieces)[i][j];
			if (!Piece::isTower(piece)) continue;
			float value = Piece::height(piece);//(Piece::height(piece) + (Piece::height(piece) == 1 ? -0.1 : 0) + 2.5 * (Piece::hasAddOn(piece)));
			value += (Piece::height(piece) == 1 ? -0.1 : 0);
			value *= (Piece::isWhiteTower(piece) ? 1 : -1);
			eval += value;
		}
	}
	return eval;
}

int Utils::getBestMoveBasic(bool isWhite, uint8_t(*pieces)[13][13], int depth)
{
	std::shared_ptr<std::vector<std::vector<BasicGenerator::xMove>>> moves;
	moves = BasicGenerator::getMoves(isWhite, pieces);

	std::vector<int> bestMoves;
	float bestEval = -99999;

	debugContainer debug;

	float newBestThreshold = 0.1;
	float bestMoveLeniency = 0.005;


	for (int j = 1; j < moves->size(); j++) {
		uint8_t boardCopy[13][13];
		std::memcpy(&boardCopy, pieces, sizeof(boardCopy));
		for (int i = 0; i < ((*moves)[j]).size(); i++) {
			boardCopy[((*moves)[j])[i].i][((*moves)[j])[i].j] ^= ((*moves)[j])[i].delta;
		}

		Utils::extraBoardData metaData = generateMetadata(&boardCopy);

		if (gameOver(isWhite, metaData) == (isWhite ? winValue::white : winValue::black)) return j;	//return if we find a forced win for us

		float posEval = alphaBeta(&boardCopy, depth, -std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(), isWhite, Utils::basicPosEval, &debug) * (isWhite ? 1 : -1);

		if (posEval > (bestEval + newBestThreshold)) {
			bestMoves.clear();
			bestMoves.emplace_back(j);
			bestEval = posEval;
		}
		else if (posEval >= bestEval - bestMoveLeniency) {
			bestMoves.emplace_back(j);
		}
	}

	print("getBestMoveBasic: ");
	print(debug.n, true);
	print(bestEval, true);
	print(bestMoves.size(), true);

	int random = rand() % bestMoves.size();
	//print(bestMoves[random]);
	//print("\n");
	return bestMoves[random];
}

int Utils::trivialBestMove(bool isWhite, uint8_t(*pieces)[13][13])
{
	std::shared_ptr<std::vector<std::vector<BasicGenerator::xMove>>> moves;
	moves = BasicGenerator::getMoves(isWhite, pieces);

	float bestEval = -99999;
	int bestMove = 0;

	int depth = 1;

	debugContainer debug;

	for (int j = 1; j < moves->size(); j++) {
		uint8_t boardCopy[13][13];
		std::memcpy(&boardCopy, pieces, sizeof(boardCopy));
		for (int i = 0; i < ((*moves)[j]).size(); i++) {
			boardCopy[((*moves)[j])[i].i][((*moves)[j])[i].j] ^= ((*moves)[j])[i].delta;
		}

		if (gameOver(isWhite, &boardCopy) == (isWhite ? winValue::white : winValue::black)) return j;

		float posEval = alphaBeta(&boardCopy, depth, -std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(), !isWhite, Utils::basicPosEval, &debug) * (isWhite ? 1 : -1);

		//print(posEval, true);

		if (posEval > bestEval) {
			bestEval = posEval;
			bestMove = j;
		}
		else if (posEval == bestEval && (rand() % 100 == 0)) {
			bestMove = j;
		}
	}

	print("\ntrivialMove: ");
	print(debug.n, true);
	print(bestEval, true);
	print(bestMove, true);

	return bestMove;
}

void Utils::evaluatePositionsThread(bool isWhite, uint8_t(*pieces)[13][13], std::shared_ptr<std::vector<std::vector<BasicGenerator::xMove>>> moves, std::vector<float>& evaluations, int depth, int threadIndex, int threadCount, debugContainer& debug) {
	int j = threadIndex;
	while (j < moves->size()) {
		uint8_t boardCopy[13][13];
		std::memcpy(&boardCopy, pieces, sizeof(boardCopy));

		for (int i = 0; i < ((*moves)[j]).size(); i++) {
			boardCopy[((*moves)[j])[i].i][((*moves)[j])[i].j] ^= ((*moves)[j])[i].delta;
		}

		Utils::extraBoardData metaData = generateMetadata(&boardCopy);

		float posEval = alphaBeta(&boardCopy, depth, -std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(), isWhite, Utils::basicPosEval, &debug) * (isWhite ? 1 : -1);
		evaluations[j] = posEval;


		j += threadCount;
	}
}


int Utils::getBestMoveThreaded(bool isWhite, uint8_t(*pieces)[13][13], int depth, int threadCount)
{
	std::shared_ptr<std::vector<std::vector<BasicGenerator::xMove>>> moves;
	moves = BasicGenerator::getMoves(isWhite, pieces);

	debugContainer debug;

	std::vector<std::thread> threads = std::vector<std::thread>();
	std::vector<float> evaluations = std::vector<float>(moves->size());

	//std::vector<int>* e = &evaluations;

	for (int i = 0; i < threadCount; i++) {
		threads.emplace_back(std::thread(&Utils::evaluatePositionsThread, isWhite, std::ref(pieces), std::ref(moves), std::ref(evaluations), depth, i, threadCount, std::ref(debug)));
		//threads.emplace_back(std::thread(&Utils::test, std::ref(evaluations)));
	}
	for (int i = 0; i < threadCount; i++) {
		threads[i].join();
	}

	/*for (int i = 0; i < evaluations.size(); i++) {
		print(evaluations[i], true);
	}

	print("------------------");*/

	std::vector<int> bestMoves;
	bestMoves.emplace_back(0);
	float bestEval = -99999;

	float newBestThreshold = 0.1;
	float bestMoveLeniency = 0.005;

	for (int i = 1; i < evaluations.size(); i++) {	//skip the null move
		float posEval = evaluations[i];
		if (posEval > (bestEval + newBestThreshold)) {
			bestMoves.clear();
			bestMoves.emplace_back(i);
			bestEval = posEval;
		}
		else if (posEval >= bestEval - bestMoveLeniency) {
			bestMoves.emplace_back(i);
		}
	}

	print("getBestMoveThreaded: ");
	print(debug.n, true);
	print(bestEval, true);
	print(bestMoves.size(), true);

	int random = rand() % bestMoves.size();
	return bestMoves[random];
}

Utils::extraBoardData Utils::generateMetadata(uint8_t(*pieces)[13][13])
{
	extraBoardData data = extraBoardData();

	for (int i = 0; i < 13; i++) {
		for (int j = 0; j < 13; j++) {
			uint8_t piece = (*pieces)[i][j];
			if ((piece & 0b00111111) == 0 || !Piece::isTower(piece)) continue;
			/*print(i, false);
			print("|", false);
			print(j, false);
			print(" -> ", false);
			print(BITSETINDEX, false);
			print(" - ", false);
			print(BITSETINDEX / 64, false);
			print(" : ", false);
			print(BITSETINDEX % 64, false);
			print(" - ", false);
			print(static_cast<unsigned long long>(1) << (BITSETINDEX % 64), true);*/
			data.colorBitboards[!Piece::isWhiteTower(piece)][BITSETINDEX / 64] += static_cast<unsigned long long>(1) << (BITSETINDEX % 64);
		}
	}

	//print(data);

	return data;
}

std::shared_ptr<std::vector<std::vector<Utils::xMove>>> Utils::getMoves_halfSplit_noPush(bool isWhite, uint8_t(*pieces)[13][13]) {
	std::shared_ptr<std::vector<std::vector<xMove>>> moves = std::make_shared<std::vector<std::vector<xMove>>>();
	moves->reserve(700);

	for (int i = 0; i < 13; i++) {
		for (int j = 0; j < 13; j++) {
			uint8_t piece = (*pieces)[i][j];
			if (Piece::height(piece) == 0 || Piece::isAddOn(piece)) continue;
			if (Piece::isWhiteTower(piece) == !isWhite) continue;

			uint8_t boardCopy[13][13];
			std::memcpy(&boardCopy, pieces, sizeof((*pieces)));
			bool visited[13][13];
			memset(visited, false, sizeof(visited));
			visited[i][j] = true;
			basicGenerator_halfSplit_noPush(moves, &boardCopy, i, j, &visited, Piece::maxSteps(piece), false, isWhite, pieces);
		}
	}

	return moves;
}

void Utils::basicGenerator_halfSplit_noPush(std::shared_ptr<std::vector<std::vector<xMove>>> moves, uint8_t(*state)[13][13], int x, int y, bool(*visited)[13][13], int remainingSteps, bool turned, bool isWhite, uint8_t(*pieces)[13][13])
{
	std::vector<xMove> move = std::vector<xMove>();
	move.reserve(10);
	for (int i = 0; i < 13; i++) {
		for (int j = 0; j < 13; j++) {
			if ((*pieces)[i][j] == (*state)[i][j]) continue;
			move.emplace_back(xMove{ i, j, ((uint8_t)(((*pieces)[i][j]) ^ ((*state)[i][j]))) });		//Creation of xor lists for moves
		}
	}
	if (moves->size() == 0 || move.size() > 0) {
		moves->emplace_back(move);
	}

	if (!turned && (((*state)[x][y] & turnPiece) != 0)) {		//Turn in place if we can and haven't yet
		uint8_t boardCopy[13][13];
		std::memcpy(&boardCopy, state, sizeof(boardCopy));
		boardCopy[x][y] ^= setTurnPiece;
		bool visitedCopy[13][13];
		std::memcpy(&visitedCopy, visited, sizeof(visitedCopy));
		basicGenerator_halfSplit_noPush(moves, &boardCopy, x, y, &visitedCopy, remainingSteps, true, isWhite, pieces);
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
			for (int splitOff = floor(Piece::height(piece) / 2.0); splitOff <= Piece::height(piece); splitOff = (splitOff < Piece::height(piece) ? Piece::height(piece) : 100)) {		//number of moved pieces
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
				basicGenerator_halfSplit_noPush(moves, &boardCopy, i, j, &visitedCopy, remainingSteps - 1, false, isWhite, pieces);
			}
		}
		else if (Piece::height(dest) > 0 && (Piece::isAddOn(dest) ? true : (Piece::colour(dest) == Piece::colour(piece)))) {		//merging
			for (int splitOff = floor(Piece::height(piece) / 2.0); splitOff <= Piece::height(piece); splitOff = (splitOff < Piece::height(piece) ? Piece::height(piece) : 100)) {
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
				basicGenerator_halfSplit_noPush(moves, &boardCopy, i, j, &visitedCopy, 0, false, isWhite, pieces);
			}
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
			captureGenerator_singleSplit(moves, &boardCopy, x, y, i, j, &visitedCopy, remainingSteps - 1, false, isWhite, pieces);
		}
	}
}

void Utils::captureGenerator_singleSplit(std::shared_ptr<std::vector<std::vector<xMove>>> moves, uint8_t(*state)[13][13], int originX, int originY, int x, int y, bool(*visited)[13][13], int remainingSteps, bool turned, bool isWhite, uint8_t(*pieces)[13][13])
{
	/*
		0
	   3 1
		2
	*/
	uint8_t origin = ((*state)[originX][originY]);

	if (!turned && (((*state)[x][y] & turnPiece) != 0)) {		//Turn in place if we can and haven't yet
		uint8_t boardCopy[13][13];
		std::memcpy(&boardCopy, state, sizeof(boardCopy));
		boardCopy[x][y] ^= setTurnPiece;
		bool visitedCopy[13][13];
		std::memcpy(&visitedCopy, visited, sizeof(visitedCopy));
		captureGenerator_singleSplit(moves, &boardCopy, originX, originY, x, y, &visitedCopy, remainingSteps, true, isWhite, pieces);
	}

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

		//piece is the position where an enemy piece used to be that we are currently taking, this value has already been wiped
		//dest is the piece we are now looking to move to, either to complete the capture or to continue the chain
		//the original piece doing the capturing still exists at originX, originY in the state array
		//splitting will be done once the end of a capture chain has been found


		//ending the chain on an empty square or a friendly tower or a single addon to merge onto
		if (Piece::height(dest) == 0 || (Piece::isTower(dest) && Piece::colour(dest) == Piece::colour(origin)) || Piece::isAddOn(dest)) {

			//We have to split less than the full tower, except if the height is 1 and we can't go over the merge limit
			int splitOff = Piece::height(origin) == 1 ? 1 : (Piece::height(origin) - 1);

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
			basicGenerator_halfSplit_noPush(moves, &boardCopy, i, j, &visitedCopy, 0, false, isWhite, pieces);		//end move
		}
		else if (remainingSteps + Piece::isBlue(origin) > 1 && Piece::isTower(dest) && Piece::colour(origin) != Piece::colour(dest)) {
			if (!Piece::isBlue(origin) && Piece::height(dest) > Piece::height(origin)) continue;		//capturing height check
			uint8_t boardCopy[13][13];
			std::memcpy(&boardCopy, state, sizeof(boardCopy));
			boardCopy[i][j] &= 0b11000000; //remove piece we're capturing
			bool visitedCopy[13][13];
			std::memcpy(&visitedCopy, visited, sizeof(visitedCopy));
			visitedCopy[i][j] = true;
			captureGenerator_singleSplit(moves, &boardCopy, originX, originY, i, j, &visitedCopy, remainingSteps - !Piece::isBlue(origin), false, isWhite, pieces);		//only decrement moves if the capturing piece doesn't have a blue addon
		}
	}
}

float Utils::alphaBeta(uint8_t(*pieces)[13][13], int depth, float alpha, float beta, bool maximizing, float (*evalFunc)(bool isWhite, uint8_t(*pieces)[13][13]), debugContainer* debug)
{
	if (debug != nullptr) debug->n++;
	if (debug != nullptr) debug->depthCounts[depth]++;


	if (depth == 0) {// || (Utils::gameOver(maximizing, pieces) != winValue::none)) {
		return evalFunc(maximizing, pieces);
	}

	extraBoardData data = generateMetadata(pieces);
	if (Utils::gameOver(maximizing, data) != winValue::none) {
		return evalFunc(maximizing, pieces);
	}

	if (maximizing) {
		float value = -std::numeric_limits<float>::infinity();//-MAXEVAL;

		//std::shared_ptr<std::vector<std::vector<Utils::xMove>>> moves;
		//moves = Utils::getMoves_halfSplit_noPush(maximizing, pieces);

		std::shared_ptr<std::vector<std::vector<BasicGenerator::xMove>>> moves;
		moves = BasicGenerator::getMoves(maximizing, pieces);

		for (int j = 1; j < moves->size(); j++) {
			uint8_t boardCopy[13][13];
			std::memcpy(&boardCopy, pieces, sizeof(boardCopy));
			for (int i = 0; i < ((*moves)[j]).size(); i++) {
				boardCopy[((*moves)[j])[i].i][((*moves)[j])[i].j] ^= ((*moves)[j])[i].delta;
			}

			float alphabeta = alphaBeta(&boardCopy, depth - 1, alpha, beta, false, evalFunc, debug);
			value = max(value, alphabeta);

			//if (value > beta) break;
			alpha = max(alpha, value);
		}
		return value;
	}
	else {
		float value = std::numeric_limits<float>::infinity();//MAXEVAL;

		std::shared_ptr<std::vector<std::vector<Utils::xMove>>> moves;
		moves = Utils::getMoves_halfSplit_noPush(maximizing, pieces);

		for (int j = 1; j < moves->size(); j++) {
			uint8_t boardCopy[13][13];
			std::memcpy(&boardCopy, pieces, sizeof(boardCopy));
			for (int i = 0; i < ((*moves)[j]).size(); i++) {
				boardCopy[((*moves)[j])[i].i][((*moves)[j])[i].j] ^= ((*moves)[j])[i].delta;
			}

			float alphabeta = alphaBeta(&boardCopy, depth - 1, alpha, beta, true, evalFunc, debug);
			value = min(value, alphabeta);

			//if (value < alpha) break;
			beta = min(beta, value);
		}

		return value;
	}
}
