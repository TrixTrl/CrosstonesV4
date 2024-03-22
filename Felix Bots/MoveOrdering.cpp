#include "MoveOrdering.h"
#include "Utility.h"
#include "Evaluation.h"

using namespace dc;

std::vector<int> MoveOrdering::makeMoveOrdering(const Move& hashMove, const Board& board, const std::vector<Move>& moveList, bool debug)
{
	if (moveScores.size() < moveList.size())
	{
		moveScores = std::vector<int>(moveList.size() + 10);
	}

	std::vector<int> indices(moveList.size());

	if (moveList.size() == 0)
		return indices;
	
	for (int i = 0; i < moveList.size(); i++)
	{
		const Move& move = moveList[i];
		if (Utility::sameMove(move, hashMove))
		{
			moveScores[i] = hashMoveScore;
			continue;
		}
		int score = 0;
		const xMove startXMove = move[0];

		if (move.size() >= 2)
		{
			const xMove endXMove = move[1];
			const uint8_t startPiece = Piece::tower(board.square[startXMove.i][startXMove.j]);
			const uint8_t endPiece = Piece::tower(board.square[endXMove.i][endXMove.j]);
			bool isCapture = false;
			//check if it is a capture
			for (int visitedIdx = 2; visitedIdx < move.size(); visitedIdx++)
			{
				const xMove curXMove = move[visitedIdx];
				const uint8_t curPiece = Piece::tower(board.square[visitedIdx][visitedIdx]);
				if (Piece::isTower(curPiece))
				{
					//check for capture
					if (Piece::isWhite(curPiece) != board.isWhiteTurn)
					{
						score += normalCaptureBias + Evaluation::pieceValue[curPiece & 0b00011111];
						isCapture = true;
					}
				}
			}
			//check for claim
			if (Piece::isAddOn(endPiece))
			{
				score += (Piece::isBlue(endPiece) ? blueClaimBias : redClaimBias);
			}
			if (!isCapture)
			{
				score += Evaluation::pieceValue[startPiece & 0b00011111];
			}
			
			int toScore = Evaluation::positionMap[(1 - Piece::isWhite(startPiece)) * 12 + (2 * Piece::isWhite(startPiece) - 1) * endXMove.j][endXMove.i];
			int fromScore = Evaluation::positionMap[(1 - Piece::isWhite(startPiece)) * 12 + (2 * Piece::isWhite(startPiece) - 1) * startXMove.j][startXMove.i];
			score += toScore - fromScore;
		}
		moveScores[i] = score;
		
	}
	
	for (int i = 0; i < moveList.size(); i++)
		indices[i] = i;

	quicksort(indices, moveScores, 0, moveList.size() - 1);
	if (debug)
	{
		for (int i = 0; i < moveList.size(); i++) {
			Utility::print(std::format("Move {}: Index: {}, Value: {}",
				i, indices[i], moveScores[i]), true);
		}
	}


	return indices;
}

void MoveOrdering::quicksort(std::vector<int>& indices, std::vector<int>& vals, int L, int R) {
	int i, j, mid, piv;
	i = L;
	j = R;
	mid = L + (R - L) / 2;
	piv = vals[mid];

	while (i<R || j>L) {
		while (vals[i] > piv)
			i++;
		while (vals[j] < piv)
			j--;

		if (i <= j) {
			swap(indices, vals, i, j);
			i++;
			j--;
		}
		else {
			if (i < R)
				quicksort(indices, vals, i, R);
			if (j > L)
				quicksort(indices, vals, L, j);
			return;
		}
	}
}

void MoveOrdering::swap(std::vector<int>& indices, std::vector<int>& vals, int x, int y) {
	int tempIndex = indices[x];
	int tempVal = vals[x];
	indices[x] = indices[y];
	indices[y] = tempIndex;
	vals[x] = vals[y];
	vals[y] = tempVal;

}