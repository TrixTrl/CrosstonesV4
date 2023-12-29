#pragma once

#include "BoardState.h"
#include <Windows.h>
#include <WinUser.h>
#include <vector>
#include <functional>


class DCTestSuite {
public:
	struct Board {
		uint8_t data[13][13];
	};

	static bool run(HWND globalHwnd, uint8_t(*display)[13][13]);
private:
	static std::vector<DCTestSuite::Board> loadExampleBoards();
	static long long doTimed(std::function<void()> action);
};