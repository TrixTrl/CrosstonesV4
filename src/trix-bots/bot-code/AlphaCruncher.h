#pragma once

#include "globals/Player.h"
#include <memory>
#include "../utils/TrixBoardState.h"
<<<<<<< HEAD
#include "../mcts/MCTS.h"
#include "../mcts/Toggles.h"
=======
#include "../utils/MCTS.h"
#include "../utils/Toggles.h"
>>>>>>> fbfe60251222b6453cb38841641b5d46a67ebea5

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