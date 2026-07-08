#include "MoveOrdering.h"
#include "felix-bots/util/Utility.h"
#include "felix-bots/evaluation/Evaluation.h"

using namespace dc;

std::vector<int> MoveOrdering::makeMoveOrdering(const XMove& hashMove, const BotBoard& board, const std::vector<XMove>& moveList, bool debug)
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
		const XMove& move = moveList[i];
		if (move == hashMove)
		{
			moveScores[i] = hashMoveScore;
			continue;
		}
		int score = 0;
		const XMoveTile startXMove = move[0];

		if (move.size() >= 2)
		{
			const XMoveTile endXMove = move[1];
			const uint8_t startPiece = Piece::tower(board.square[startXMove.i][startXMove.j]);
			const uint8_t endPiece = Piece::tower(board.square[endXMove.i][endXMove.j]);
			
			//check if it is a capture
			bool isCapture = false;
			for (int visitedIdx = 2; visitedIdx < move.size(); visitedIdx++)
			{
				const XMoveTile curXMove = move[visitedIdx];
				const uint8_t curPiece = Piece::tower(board.square[visitedIdx][visitedIdx]);
				if (Piece::isTower(curPiece))
				{
					//check for capture
					if (Piece::isWhite(curPiece) != board.isWhiteTurn)
					{
						score += evaluation.getEffectivePieceWorth(board, curPiece, curXMove.i, curXMove.j);
						isCapture = true;
					}
				}
			}
			score += normalCaptureBias * isCapture;

			//check for claim
			if (Piece::isAddOn(endPiece))
			{
				score += (Piece::isBlue(endPiece) ? blueClaimBias : redClaimBias);
			}
			for (int visitedIdx = 0; visitedIdx < move.size(); visitedIdx++)
			{
				const XMoveTile curXMove = move[visitedIdx];
				const uint8_t pieceBefore = board.square[visitedIdx][visitedIdx];
				const uint8_t pieceAfter = pieceBefore ^ curXMove.delta;
				int scoreBefore = evaluation.getEffectivePieceWorth(board, pieceBefore, curXMove.i, curXMove.j);
				int scoreAfter = evaluation.getEffectivePieceWorth(board, pieceAfter, curXMove.i, curXMove.j);
				score += scoreAfter - scoreBefore;
			}
			/*
			if (!isCapture)
			{
				score += Evaluation::pieceValue[startPiece & 0b00011111];
			}
			
			int toScore = evaluation.getEffectivePieceWorth(board, startPiece, endXMove.i, endXMove.j);
			int fromScore = evaluation.getEffectivePieceWorth(board, startPiece, startXMove.i, startXMove.j);
			score += toScore - fromScore;
			*/
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