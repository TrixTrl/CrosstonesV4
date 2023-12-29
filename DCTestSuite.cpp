#include "Felix Bots/Deepchad.h"
#include "Felix Bots/DCUtils.h"
#include "DCTestSuite.h"
#include <Windows.h>
#include <WinUser.h>
#include <string>
#include <chrono>
#include <format>
#include <functional>

bool DCTestSuite::run(HWND globalHwnd, uint8_t(*display)[13][13])
{
	//InvalidateRect(globalHwnd, NULL, NULL);

	Deepchad deepchad = Deepchad();
	/*Player* p2 = new TestBotRnd();
	std::bitset<3> gamemode(0b111);
	GameMaster gameMaster(gamemode, p1, p2, 3000, 0, &(displayBoard[0]));

	gameMaster.play(globalHwnd);*/

	std::vector<DCTestSuite::Board> boards = loadExampleBoards();

	std::memcpy(display, &(boards.at(0).data), sizeof(boards.at(0).data));
	InvalidateRect(globalHwnd, NULL, NULL);

	/* Display the board
	std::memcpy(display, &(boards.at(0).data), sizeof(boards.at(0).data));
	InvalidateRect(globalHwnd, NULL, NULL);
	*/
	auto execTime = doTimed([&]()
		{
			for (auto& board : boards) {
				uint8_t boardCopy[13][13];
				std::memcpy(&boardCopy, &(board.data), sizeof(boardCopy));

				deepchad.getMoveToPlay(&boardCopy, true, 3000);
			}
		});
	DCUtils::print(std::format("Deepchad Time: {}", execTime), true);
	return true;
}

std::vector<DCTestSuite::Board> DCTestSuite::loadExampleBoards() {
	std::vector<DCTestSuite::Board> boards;
	BoardState bsService;

	const std::vector<std::string> positions = {
		"b-10000 b-10012 b-11200 b-11212 b-10606 r-10006 r-11206 -W30512 -W30510 -B30502 -B30500 -W30712 -W30710 -B30702 -B30700 11111111111111111111", //Dragon start
		"b-10000 b-10012 b-11200 b-11212 -W30512 -W30712 -W10808 rW30406 bW40506 -B30700 -B10404 rB50704 -B30402 -B10602 11110110101111101111", //self played position
		"-B30500 b-11200 bW20204 -W10304 -B30804 -B11204 -B20410 -W11210 bW20212 r-10412 -W10512 -W20712 11110111110111111010", //"unsolved" endgame
		"b-10000 b-10012 b-11200 b-11212 rW20006 r-11206 -B10206 -B20204 -B30500 -B30700 -W30512 -W30712 -W10605 -W10607 -W10808 -W20809 -B21104 11110111111111111111", //seagull gambit
		"-B10700 -W10803 -W10602 00000000000000000000" //win in 4 ply
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

long long DCTestSuite::doTimed( std::function<void()> action)
{
	auto start = std::chrono::system_clock::now();
	action();
	auto stop = std::chrono::system_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
	
	return duration.count();
}