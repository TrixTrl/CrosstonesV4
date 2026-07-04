#pragma once

#include "globals/Player.h"
#include <memory>
#include "../utils/TrixBoardState.h"
#include "../mcts/MCTS.h"
#include "../mcts/Toggles.h"

class AlphaCruncher : public Player
{
public:
	AlphaCruncher(int thinkingTime) : thinkingTime(thinkingTime) {};
	void getMoveToPlay(uint8_t(*board)[13][13], bool isWhite, int endTime);
	bool listensToKeyInputs() override {
		return false;
	}
    int thinkingTime = 10;
    std::map<std::string, Node> tree = std::map<std::string, Node>();
};