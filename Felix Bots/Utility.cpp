#include "Utility.h"
#include "../Globals/Piece.h"
#include <vector>

using namespace std;
using namespace dc;

std::vector<BasicGenerator::xMove> Utility::nullMove;
Utility::_init Utility::_initializer;


void Utility::applyXMove(uint8_t(*state)[13][13], Move& move)
{
	for (auto& xMove : move)
	{
		(*state)[xMove.i][xMove.j] ^= xMove.delta;
	}
}

bool Utility::sameMove(const Move& moveA, const Move& moveB)
{
	if (moveA.size() != moveB.size())
		return false;

	for (int i = 0; i < moveA.size(); i++)
	{
		if (!sameXMove(moveA[i], moveB[i]))
			return false;
	}
	return true;
}

bool Utility::sameXMove(const BasicGenerator::xMove& moveA, const BasicGenerator::xMove& moveB)
{
	return moveA.i == moveB.i && moveA.j == moveB.j && moveA.delta == moveB.delta;
}
