#pragma once

#include "globals/Player.h"
#include "TranspositionTable.h"

class TheFirst : public Player
{
public:
	TheFirst(int setDepth) : depth(setDepth) { ZobristHasher::fillValues(); };
	void getMoveToPlay(uint8_t(*board)[13][13], bool isWhite, int endTime);
	bool listensToKeyInputs() override {
		return false;
	}

	int depth = 2;
	int threadCount = 1;
};

