#pragma once

#ifndef BOARDSTATE_H
#define BOARDSTATE_H

#include <cstdint>
#include <bitset>
#include <vector>

class BoardState
{
public:
	BoardState();
	void rst(std::bitset<3>& tps);
	void print();

	void copyBoard(uint8_t(*dest)[13][13]);

	struct xMove;

	std::vector<std::vector<xMove>>* getMoves(bool isWhite);
	void basicGenerator(std::vector<std::vector<xMove>>* moves, uint8_t(*state)[13][13], int x, int y, bool(*visited)[13][13], int remainingSteps, bool turned);

	void unsafeMakeMove(std::vector<xMove>* move);

	static const uint8_t turnPiece = 128;
	static const uint8_t setTurnPiece = 64;
private:
	uint8_t pieces[13][13];
};

#endif