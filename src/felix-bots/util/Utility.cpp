#include "felix-bots/util/Utility.h"
#include "globals/Piece.h"
#include <vector>

using namespace std;
using namespace dc;

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

bool Utility::isStartingBoard(uint8_t(*state)[13][13])
{
	return ((*state)[5][10] == Piece::whiteNormal3)
		&& ((*state)[7][10] == Piece::whiteNormal3)
		&& ((*state)[5][12] == Piece::whiteNormal3)
		&& ((*state)[7][12] == Piece::whiteNormal3)
		&& ((*state)[5][0] == Piece::blackNormal3)
		&& ((*state)[7][0] == Piece::blackNormal3)
		&& ((*state)[5][2] == Piece::blackNormal3)
		&& ((*state)[7][2] == Piece::blackNormal3)
		&& ((*state)[0][0] == Piece::blue1)
		&& ((*state)[12][0] == Piece::blue1)
		&& ((*state)[0][12] == Piece::blue1)
		&& ((*state)[12][12] == Piece::blue1)
		&& ((*state)[6][6] == Piece::blue1)
		&& ((*state)[0][6] == Piece::red1)
		&& ((*state)[12][6] == Piece::red1);
}
bool Utility::isLeftMove(Move& move)
{
	return move[0].i <= 6;
}
Move Utility::mirrorMoveLR(Move& move)
{
	Move newMove = std::vector<BasicGenerator::xMove>();
	for (const auto& xMove : move)
		newMove.emplace_back(12 - xMove.i, xMove.j, xMove.delta);
	return newMove;
}