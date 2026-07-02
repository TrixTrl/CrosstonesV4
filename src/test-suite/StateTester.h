#pragma once
#include "game-suite/Board.h"
#include <string>

class StateTester
{
public:
	StateTester(Board bs);
	bool staticLegality();
	bool fullTest();
	int pieceCount();
	int redCount();
	int blueCount();

	bool checkAllMoves();

	uint8_t pieces[13][13];
	std::string error = "";
	Board BS;
};

