#pragma once

#include "../Globals/Player.h"

class TheFirst : public Player
{
public:
	TheFirst(int setDepth) : depth(setDepth) {};
	void getMoveToPlay(uint8_t(*board)[13][13], bool isWhite, int endTime);
	bool listensToKeyInputs() override {
		return false;
	}

	int depth = 2;
};

