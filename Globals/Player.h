#pragma once
#include <cstdint>
#include "PlayerInputKey.h"

class Player {
public:

	virtual void getMoveToPlay(uint8_t(*board)[13][13], bool isWhite, int endTime) {};
	
	virtual bool listensToKeyInputs() 
	{
		return false;
	};
	virtual void keyDown(PlayerInputKey key) {}
};