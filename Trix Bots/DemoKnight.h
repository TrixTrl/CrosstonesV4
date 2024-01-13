#pragma once

#include "../Globals/Player.h"
#include "Utils.h"

class DemoKnight : public Player
{
	void getMoveToPlay(uint8_t(*board)[13][13], bool isWhite, int endTime);
	bool listensToKeyInputs() override {
		return false;
	}

	int n = 0;

	int positions[8][4] = { {7, 10, 8, 8}, { 8, 8, 8, 4 }, { 8, 4, 10, 2 },
							{5, 10, 4, 8}, { 4, 8, 4, 4 }, { 4, 4, 2, 2 },
							{7, 12, 9, 12}, {5, 12, 3, 12} };		//[color][moveNr][0-4 x1, y1, x2, y1]
};

