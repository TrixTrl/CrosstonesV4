#include "Felix Bots/Deepchad.h"
#include "Felix Bots/Board.h"
#include "Felix Bots/Utility.h"
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

	Deepchad deepchad = Deepchad(1);
	/*Player* p2 = new TestBotRnd();
	std::bitset<3> gamemode(0b111);
	GameMaster gameMaster(gamemode, p1, p2, 3000, 0, &(displayBoard[0]));

	gameMaster.play(globalHwnd);*/

	std::vector<dc::Board> boards = loadExampleBoards();

	std::memcpy(display, &(boards.at(3).square), sizeof(boards.at(3).square));
	InvalidateRect(globalHwnd, NULL, NULL);

	/* Display the board
	std::memcpy(display, &(boards.at(0).data), sizeof(boards.at(0).data));
	InvalidateRect(globalHwnd, NULL, NULL);
	*/
	auto execTime = doTimed([&]()
		{
			//for (auto& board : boards) {
			dc::Board& board = boards.at(3);
			uint8_t boardCopy[13][13];
			std::memcpy(&boardCopy, &(board.square), sizeof(boardCopy));

			deepchad.getMoveToPlay(&boardCopy, true, 3000);

			std::memcpy(display, &(boardCopy), sizeof(boardCopy));
			InvalidateRect(globalHwnd, NULL, NULL);
			//}
		});
	dc::Utility::print(std::format("Deepchad Time: {}\n", execTime), true);


	std::function<std::vector<Move>(uint8_t(*)[13][13], bool)> newMoveGenerator = 
		[](uint8_t(*pieces)[13][13], bool isWhite)
		{
			std::vector<Move> moves;
			dc::MoveGenerator::getMovesStatic(&moves, pieces, isWhite, false);
			return moves;
		};
	std::function<std::vector<Move>(uint8_t(*)[13][13], bool)> oldMoveGenerator =
		[](uint8_t(*pieces)[13][13], bool isWhite)
		{
			std::shared_ptr<std::vector<Move>> moves = BasicGenerator::getMoves(isWhite, pieces);
			return std::vector<Move>(moves->begin() + 1, moves->end());
		};


	// The first board should be a dragon start for consistent results
	// Assuming no passing

	const int perftDepth = 3;
	dc::Utility::print(std::format("Executing Perft {}:", perftDepth), true);

	int nodes = 0;
	auto perftTime = doTimed([&]()
		{
			for (int i = 0; i < 10; i++)
				nodes = boards[0].bulk_perft(perftDepth, newMoveGenerator);
		});
	dc::Utility::print(std::format(
			"Perft Nodes: {}\nPerft Time: {} ms", 
			nodes,
			perftTime
		), true);
	/* 
		BENCHMARK: Perft 4:
		Perft Nodes: 315404724
		Perft Time: 113650 ms

		Perft 3 * 10: old
		Perft Nodes: 2345176
		Perft Time: 16436 ms
		New Time: 9006 ms
	*/
	return true;
}

std::vector<dc::Board> DCTestSuite::loadExampleBoards() {
	std::vector<dc::Board> boards;
	BoardState bsService;

	const std::vector<std::pair<std::string, bool>> positions = {
		std::pair("b-10000 b-10012 b-11200 b-11212 b-10606 r-10006 r-11206 -W30512 -W30510 -B30502 -B30500 -W30712 -W30710 -B30702 -B30700 11111111111111111111", true), //Dragon start
		std::pair("b-10000 b-10012 b-11200 b-11212 -W30512 -W30712 -W10808 rW30406 bW40506 -B30700 -B10404 rB50704 -B30402 -B10602 11110110101111101111", true), //self played position
		std::pair("-B30500 b-11200 bW20204 -W10304 -B30804 -B11204 -B20410 -W11210 bW20212 r-10412 -W10512 -W20712 11110111110111111010", true), //"unsolved" endgame
		std::pair("b-10000 b-10012 b-11200 b-11212 rW20006 r-11206 -B10206 -B20204 -B30500 -B30700 -W30512 -W30712 -W10605 -W10607 -W10808 -W20809 -B21104 11110111111111111111", false), //seagull gambit
		std::pair("-B10700 -W10803 -W10602 00000000000000000000", true) //win in 4 ply
	};

	// convert positions
	for (int i = 0; i < positions.size(); i++) {
		dc::Board board;
		board.isWhiteTurn = positions[i].second;
		bsService.loadPos(positions[i].first);
		bsService.copyBoard(&(board.square));
		boards.push_back(board);
	}

	return boards;
}

u64 DCTestSuite::doTimed( std::function<void()> action)
{
	auto start = std::chrono::system_clock::now();
	action();
	auto stop = std::chrono::system_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
	
	return duration.count();
}