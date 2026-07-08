#include "felix-bots/util/Utility.h"
#include "globals/Piece.h"
#include <vector>
#include <data/XMove.h>

using namespace std;
using namespace dc;

void Utility::applyXMove(GamePosition& state, XMove& move)
{
	for (auto& xMove : move)
	{
		state[xMove.i][xMove.j] ^= xMove.delta;
	}
}

bool Utility::isStartingBoard(const GamePosition& state)
{
	return (state[5][10] == Piece::whiteNormal3)
		&& (state[7][10] == Piece::whiteNormal3)
		&& (state[5][12] == Piece::whiteNormal3)
		&& (state[7][12] == Piece::whiteNormal3)
		&& (state[5][0] == Piece::blackNormal3)
		&& (state[7][0] == Piece::blackNormal3)
		&& (state[5][2] == Piece::blackNormal3)
		&& (state[7][2] == Piece::blackNormal3)
		&& (state[0][0] == Piece::blue1)
		&& (state[12][0] == Piece::blue1)
		&& (state[0][12] == Piece::blue1)
		&& (state[12][12] == Piece::blue1)
		&& (state[6][6] == Piece::blue1)
		&& (state[0][6] == Piece::red1)
		&& (state[12][6] == Piece::red1);
}
bool Utility::isLeftMove(XMove& move)
{
	return move[0].i <= 6;
}
XMove Utility::mirrorMoveLR(XMove& move)
{
	XMove newMove = XMove();
	for (const auto& xMove : move)
		newMove.emplace_back(12 - xMove.i, xMove.j, xMove.delta);
	return newMove;
}