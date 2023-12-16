#pragma once
#include "../CrosstonesV4/BoardState.h"
#include <string>

class StateTester
{
public:
	StateTester(BoardState bs);
	bool staticLegality();
	bool fullTest();
	int pieceCount();
	int redCount();
	int blueCount();

	bool checkAllMoves();

	uint8_t pieces[13][13];
	std::string error = "";
	BoardState BS;
};

