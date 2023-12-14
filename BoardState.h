#pragma once

#ifndef BOARDSTATE_H
#define BOARDSTATE_H

#include <cstdint>
#include <bitset>
#include <vector>
#include <memory>

class BoardState
{
public:
	BoardState();
	void rst(std::bitset<3>& tps);

	void copyBoard(uint8_t(*dest)[13][13]);

	struct xMove;

	std::shared_ptr<std::vector<std::vector<xMove>>> getMoves(bool isWhite);
	void basicGenerator(std::shared_ptr<std::vector<std::vector<xMove>>> moves, uint8_t(*state)[13][13], int x, int y, bool(*visited)[13][13], int remainingSteps, bool turned, bool isWhite);
	void captureGenerator(std::shared_ptr<std::vector<std::vector<xMove>>> moves, uint8_t(*state)[13][13], int originX, int originY, int x, int y, bool(*visited)[13][13], int remainingSteps, bool isWhite);

	void unsafeMakeMove(std::vector<xMove>* move);
	void makeMove(std::vector<xMove>* move, bool isWhiteTurn);

	static const uint8_t turnPiece = 128;
	static const uint8_t setTurnPiece = 64;
private:
	uint8_t pieces[13][13];
};

#endif