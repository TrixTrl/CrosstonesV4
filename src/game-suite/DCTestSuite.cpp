#include "felix-bots/Deepchad.h"
#include "felix-bots/board/BotBoard.h"
#include "felix-bots/util/Utility.h"
#include "DCTestSuite.h"

#include <string>
#include <chrono>
#include <format>
#include <functional>

bool DCTestSuite::run(std::function<void()> onBoardChanged, uint8_t(*display)[13][13])
{
	Deepchad deepchad = Deepchad(1);

	std::vector<dc::BotBoard> boards = loadExampleBoards();

	std::memcpy(display, &(boards.at(3).square), sizeof(boards.at(3).square));
	if (onBoardChanged) onBoardChanged();

	auto execTime = doTimed([&]()
		{
			dc::BotBoard& board = boards.at(3);
			uint8_t boardCopy[13][13];
			std::memcpy(&boardCopy, &(board.square), sizeof(boardCopy));

			deepchad.getMoveToPlay(&boardCopy, true, 3000);

			std::memcpy(display, &(boardCopy), sizeof(boardCopy));
			if (onBoardChanged) onBoardChanged();
		});
	dc::Utility::print(std::format("Deepchad Time: {}\n", execTime), true);


	std::function<std::vector<XMove>(const GamePosition&, bool)> newMoveGenerator = 
		[](const GamePosition& pieces, bool isWhite)
		{
			std::vector<XMove> moves;
			dc::MoveGenerator::getMovesStatic(&moves, pieces, isWhite, false);
			return moves;
		};
	std::function<std::vector<XMove>(const GamePosition&, bool)> oldMoveGenerator =
		[](const GamePosition& pieces, bool isWhite)
		{
			std::shared_ptr<std::vector<XMove>> moves = BasicGenerator::getMoves(isWhite, &pieces.pieces);
			return std::vector<XMove>(moves->begin() + 1, moves->end());
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
	return true;
}

std::vector<dc::BotBoard> DCTestSuite::loadExampleBoards() {
	std::vector<dc::BotBoard> boards;
	Board bsService;

	const std::vector<std::pair<std::string, bool>> positions = {
		std::pair("b-10000 b-10012 b-11200 b-11212 b-10606 r-10006 r-11206 -W30512 -W30510 -B30502 -B30500 -W30712 -W30710 -B30702 -B30700 11111111111111111111", true), //Dragon start
		std::pair("b-10000 b-10012 b-11200 b-11212 -W30512 -W30712 -W10808 rW30406 bW40506 -B30700 -B10404 rB50704 -B30402 -B10602 11110110101111101111", true), //self played position
		std::pair("-B30500 b-11200 bW20204 -W10304 -B30804 -B11204 -B20410 -W11210 bW20212 r-10412 -W10512 -W20712 11110111110111111010", true), //"unsolved" endgame
		std::pair("b-10000 b-10012 b-11200 b-11212 rW20006 r-11206 -B10206 -B20204 -B30500 -B30700 -W30512 -W30712 -W10605 -W10607 -W10808 -W20809 -B21104 11110111111111111111", false), //seagull gambit
		std::pair("-B10700 -W10803 -W10602 00000000000000000000", true) //win in 4 ply
	};

	// convert positions
	for (int i = 0; i < positions.size(); i++) {
		dc::BotBoard board;
		board.isWhiteTurn = positions[i].second;
		bsService.loadPosition(positions[i].first);
		bsService.copyPositionTo(board.square);
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