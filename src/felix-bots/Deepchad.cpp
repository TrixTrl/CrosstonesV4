#include "Deepchad.h"
#include "felix-bots/util/Utility.h"
#include "test-bots/BasicGenerator.h"
#include "globals/Player.h"
#include "globals/Piece.h"
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
	
	searcher.startSearch();

	auto result = searcher.getSearchResult();

	//Ensure first move of a game is left handed
	if (isWhite && Utility::isStartingBoard(state) && !Utility::isLeftMove(result.first))
	{
		result.first = Utility::mirrorMoveLR(result.first);
		Utility::print("Mirroring Move to the left.", true);
	}

	//if (result.first != Utility::nullMove)
	Utility::applyXMove(state, result.first);

	//this_thread::sleep_for(chrono::milliseconds(1000));
}
