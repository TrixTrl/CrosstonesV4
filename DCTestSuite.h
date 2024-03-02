#pragma once

#include "BoardState.h"
#include "Felix Bots/Board.h"
#include "Felix Bots/Utility.h"
#include <Windows.h>
#include <WinUser.h>
#include <vector>
#include <functional>


class DCTestSuite {
public:
	static bool run(HWND globalHwnd, uint8_t(*display)[13][13]);
private:
	static std::vector<dc::Board> loadExampleBoards();
	static dc::u64 doTimed(std::function<void()> action);
};