#pragma once

#include "Board.h"
#include <bitset>
#include <chrono>
#include "globals/Player.h"
#include "globals/PlayerInputKey.h"
#include "fix_win32_compatibility.h"
#include <WinUser.h>
#include <thread>

class GameMaster {
private:
	Board bs;
	bool isWhiteTurn = true;
	Player* players[2];
	int timeControl;
	bool timeEnforcement[2];
	GamePosition& displayBoard;

public:
	GameMaster(std::bitset<3>& gamemode, Player* p1, Player* p2, int timeControl, int enforceTime, GamePosition& displayBoard);
	void play(std::stop_token stopToken, HWND globalHwnd, bool whiteToStart = true);
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