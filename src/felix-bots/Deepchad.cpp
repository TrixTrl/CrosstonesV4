#include "Deepchad.h"
#include "felix-bots/util/Utility.h"
#include "game-suite/GamePosition.h"
#include "test-bots/BasicGenerator.h"
#include "globals/Player.h"
#include "globals/Piece.h"
#include <algorithm>
#include <iostream>
#include <thread>
#include <chrono>
#include <limits>
#include <cstring>

using namespace std;

// should only handle time controll and alike
void Deepchad::getMoveToPlay(uint8_t(*state)[13][13], bool isWhite, int endTime)
{
	GamePosition gp;
	std::memcpy(&gp, state, sizeof(gp));
	board.initialize(gp, isWhite);
	
	searcher.startSearch();

	auto result = searcher.getSearchResult();

	//Ensure first move of a game is left handed
	if (isWhite && Utility::isStartingBoard(gp) && !Utility::isLeftMove(result.first))
	{
		result.first = Utility::mirrorMoveLR(result.first);
		Utility::print("Mirroring Move to the left.", true);
	}

	Utility::applyXMove(gp, result.first);
	std::memcpy(state, &gp, sizeof(gp));
}
