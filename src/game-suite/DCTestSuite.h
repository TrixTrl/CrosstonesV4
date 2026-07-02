#pragma once

#include "Board.h"
#include "felix-bots/BotBoard.h"
#include "felix-bots/Utility.h"
#include "fix_win32_compatibility.h"
#include <WinUser.h>
#include <vector>
#include <functional>


class DCTestSuite {
public:
	static bool run(HWND globalHwnd, uint8_t(*display)[13][13]);
private:
	static std::vector<dc::BotBoard> loadExampleBoards();
	static dc::u64 doTimed(std::function<void()> action);
};