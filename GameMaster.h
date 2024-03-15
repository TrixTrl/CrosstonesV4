#pragma once

#include "BoardState.h"
#include <bitset>
#include <chrono>
#include "Globals/Player.h"
#include "Globals/PlayerInputKey.h"
#include <Windows.h>
#include <WinUser.h>


class GameMaster {
private:
	BoardState bs;
	bool isWhiteTurn = true;
	Player* players[2];
	int timeControl;
	bool timeEnforcement[2];
	uint8_t (*displayBoardPointer)[13][13];

public:
	GameMaster(std::bitset<3>& gamemode, Player* p1, Player* p2, int timeControl, int enforceTime, uint8_t (*displayBoard)[13][13]);
	void play(HWND globalHwnd);
	void loadPos(std::string str);
	void notifyPlayersKeyDown(PlayerInputKey key);
	void notifyPlayersClicked(bool isLeft, POINT gridPos);
};

class Time {
public:
	static int millis() {
		return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	}
};