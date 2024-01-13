#pragma once

#include "../Globals/Player.h"
#include "Utility.h"
#include "../Test Bots/BasicGenerator.h"
#include <vector>
#include "Searcher.h"
#include "Board.h"

using namespace dc;

class Deepchad : public Player
{
public:

	Board board;
	Searcher searcher;

	Deepchad() : board(), searcher(board) {}

	void getMoveToPlay(uint8_t(*board)[13][13], bool isWhite, int endTime);

	bool listensToKeyInputs() override {
		return false;
	}
};


