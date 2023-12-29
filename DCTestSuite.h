#pragma once

#include "BoardState.h"
#include <Windows.h>
#include <WinUser.h>
#include <vector>


class DCTestSuite {
public:
	struct Board {
		uint8_t data[13][13];
	};

	static bool run(HWND globalHwnd);
private:
	static std::vector<DCTestSuite::Board> loadExampleBoards();
};