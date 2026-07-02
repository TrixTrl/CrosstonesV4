#pragma once

#include "globals/Player.h"
#include "Utility.h"
#include "test-bots/BasicGenerator.h"
#include <vector>
#include "Searcher.h"
#include "BotBoard.h"

using namespace dc;

class Deepchad : public Player
{
public:

	BotBoard board;
	Searcher searcher;

	Deepchad(int maxDepth = 3) : board(), searcher(board, maxDepth) {}

	void getMoveToPlay(uint8_t(*board)[13][13], bool isWhite, int endTime);

	bool listensToKeyInputs() override {
		return false;
	}
};


