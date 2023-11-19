#pragma once

#ifndef BOARDSTATE_H
#define BOARDSTATE_H

#include <cstdint>
#include <bitset>

class BoardState
{
public:
	BoardState();
	void rst(std::bitset<3>& tps);
	void print();

	void copyBoard(uint8_t(*dest)[13][13]);

	static const int turnPiece = 128;
	static const int setTurnPiece = 64;
private:
	uint8_t pieces[13][13];
};

#endif