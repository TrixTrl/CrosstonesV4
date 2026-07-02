#pragma once

#ifndef BOARDSTATE_H
#define BOARDSTATE_H

#include <cstdint>
#include <bitset>
#include <vector>
#include <memory>

class Board
{
public:
	Board();
	void rst(std::bitset<3>& tps);
	void wipe();

	void copyBoard(uint8_t(*dest)[13][13]) const;

	struct xMove {
		int i;
		int j;
		uint8_t delta;
		xMove(int I, int J, uint8_t D) : i(I), j(J), delta(D) {}
	};

	std::shared_ptr<std::vector<std::vector<xMove>>> getMoves(bool isWhite) const;
	void basicGenerator(std::shared_ptr<std::vector<std::vector<xMove>>> moves, uint8_t(*state)[13][13], int x, int y, bool(*visited)[13][13], int remainingSteps, bool turned, bool isWhite) const;
	void captureGenerator(std::shared_ptr<std::vector<std::vector<xMove>>> moves, uint8_t(*state)[13][13], int originX, int originY, int x, int y, bool(*visited)[13][13], int remainingSteps, bool turned, bool isWhite) const;

	void unsafeMakeMove(std::vector<xMove>* move);
	int makeMove(std::vector<xMove>* move, bool isWhiteTurn);	//return -1 for invalid, 0 for passing, and 1 for any other valid move
	int makeMove(uint8_t (*newState)[13][13], bool isWhiteTurn);

	enum class winValue {
		none,
		white,
		black,
		draw
	};

	winValue gameOver(bool isWhite);

	static const uint8_t hasTurnPiece = 128;
	static const uint8_t setTurnPiece = 64;

	std::string dumpPos();
	void loadPos(std::string str);

	std::vector<std::string> gameRecord;
	void dumpGame();
private:
	uint8_t pieces[13][13];
};

#endif