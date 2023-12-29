#include "Felix Bots/Deepchad.h"
#include "Felix Bots/DCUtils.h"
#include "DCTestSuite.h"
#include <Windows.h>
#include <WinUser.h>
#include <string>
#include <chrono>
#include <format>

bool DCTestSuite::run(HWND globalHwnd)
{
	
	Deepchad deepchad = Deepchad();
	/*Player* p2 = new TestBotRnd();
	std::bitset<3> gamemode(0b111);
	GameMaster gameMaster(gamemode, p1, p2, 3000, 0, &(displayBoard[0]));

	gameMaster.play(globalHwnd);*/

	std::vector<DCTestSuite::Board> boards = loadExampleBoards();
	
	auto start = std::chrono::high_resolution_clock::now();
	auto stop = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);

	DCUtils::print(std::format("Time taken: {}", duration.count()), true);
	return true;
}

std::vector<DCTestSuite::Board> DCTestSuite::loadExampleBoards() {
	std::vector<DCTestSuite::Board> boards;
	BoardState bsService;

	const std::vector<std::string> positions = {
		"-B10700 -W10803 -W10602 00000000000000000000"
	};

	// convert positions
	for (int i = 0; i < positions.size(); i++) {
		Board board;
		bsService.loadPos(positions[i]);
		bsService.copyBoard(&(board.data));
		boards.push_back(board);
	}

	return boards;
}