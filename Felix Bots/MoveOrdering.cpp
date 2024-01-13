#include "MoveOrdering.h"
#include "Utility.h"

using namespace dc;

void MoveOrdering::orderMoves(std::vector<Move>& moveList, Move& bestMoveByValue)
{

	if (bestMoveByValue.size() == 0)
		return;
	// Find the best move in the list and rotate it to the front

	auto pivot = std::find_if(moveList.begin(),
		moveList.end(),
		[&bestMoveByValue](const Move& s) -> bool {
			return Utility::sameMove(s, bestMoveByValue);
		});
	if (pivot != moveList.end()) {
		std::rotate(moveList.begin(), pivot, pivot + 1);
	}



}