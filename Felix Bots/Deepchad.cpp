#include "Deepchad.h"
#include "Utility.h"
#include "../Test Bots/BasicGenerator.h"
#include "../Globals/Player.h"
#include "../Globals/Piece.h"
#include <algorithm>
#include <iostream>
#include <thread>
#include <chrono>
#include <limits>

using namespace std;

// should only handle time controll and alike
void Deepchad::getMoveToPlay(uint8_t(*state)[13][13], bool isWhite, int endTime)
{
	board.initialize(state, isWhite);

	shared_ptr<vector<Move>> moves =
		BasicGenerator::getMoves(isWhite, &board.square);
	
	// remove the passing move
	moves->erase(moves->begin());
	
	searcher.startSearch(*moves);

	auto result = searcher.getSearchResult();

	//if (result.first != Utility::nullMove)
	Utility::applyXMove(state, result.first);

	//this_thread::sleep_for(chrono::milliseconds(100));
}
