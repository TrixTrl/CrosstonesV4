#pragma once

#include "BoardState.h"
#include "felix-bots/Board.h"
#include "felix-bots/Utility.h"
#include "fix_win32_compatibility.h"
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