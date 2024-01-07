#pragma once

#include "BoardState.h"
#include "Felix Bots/Board.h"
#include <Windows.h>
#include <WinUser.h>
#include <vector>
#include <functional>


class DCTestSuite {
public:
	static bool run(HWND globalHwnd, uint8_t(*display)[13][13]);
private:
	static std::vector<dc::Board> loadExampleBoards();
	static long long doTimed(std::function<void()> action);
};