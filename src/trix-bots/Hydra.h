#pragma once

#include "globals/Player.h"
#include "TranspositionTable.h"

class Hydra : public Player
{
public:
	Hydra(int setDepth, int setThreads) : depth(setDepth), threadCount(setThreads) { ZobristHasher::fillValues(); };
	void getMoveToPlay(uint8_t(*board)[13][13], bool isWhite, int endTime);
	bool listensToKeyInputs() override {
		return false;
	}

	int depth = 2;
	int threadCount = 1;
};

