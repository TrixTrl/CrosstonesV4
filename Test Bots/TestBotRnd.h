#pragma once

#include "../Globals/Player.h"

class TestBotRnd : public Player
{
	void getMoveToPlay(uint8_t(*board)[13][13], bool isWhite, int endTime);

	bool listensToKeyInputs() override {
		return false;
	}
};

