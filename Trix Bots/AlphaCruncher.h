#pragma once

#include "../Globals/Player.h"
#include <memory>
#include "../BoardState.h"
#include "MCTS.h"

class AlphaCruncher : public Player
{
public:
	AlphaCruncher(int thinkingTime) : thinkingTime(thinkingTime) {};
	void getMoveToPlay(uint8_t(*board)[13][13], bool isWhite, int endTime);
	bool listensToKeyInputs() override {
		return false;
	}
    int thinkingTime = 10;
    map<string, Node> tree = map<string, Node>();
};