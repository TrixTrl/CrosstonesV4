#pragma once
#include <cstdint>

class Player {
public:
	virtual void getMoveToPlay(uint8_t(*board)[13][13], bool isWhite, int endTime) {};
};