#include "Evaluation.h"
#include "Searcher.h"
#include "GameResult.h"
#include "Utility.h"
#include "../Globals/Piece.h"

using namespace dc;

// The score that's returned is given from the perspective of whoever's turn it is to move.
// So a positive score means the player who's turn it is to move has an advantage, while a negative score indicates a disadvantage.
int Evaluation::evaluate(Board& board)
{
	int8_t color = 2 * board.isWhiteTurn - 1;

	switch (board.gameResult)
	{
	case GameResult::WhiteHasWon:
		return Searcher::immediateWinScore * color;
	case GameResult::BlackHasWon:
		return Searcher::immediateWinScore * -color;
	case GameResult::Draw:
		return 0;
	case GameResult::InProgress:
		break;
	}

	int blackWhitePoints[2] = { 0, 0 }; // 0: points for black; 1: points for white
	for (int x = 0; x < 13; x++)
	{
		for (int y = 0; y < 13; y++)
		{
			uint8_t content = board.square[x][y];
			if (Piece::isTower(content))
			{
				int blackWhiteIndex = (Piece::isWhite(content) ? 1 : 0);
				int pieceVal = (Piece::height(content)
					+ (Piece::isBlue(content) ? 1 : 0)
					+ (Piece::isRed(content) ? 3 : 0));
				blackWhitePoints[blackWhiteIndex] += pieceVal * materialWeight 
					+ pieceValue[content & 0b00011111] * positionMap[x][y] * positionWeight;
			}
		}
	}
	int relativeStrength = 5 * ((blackWhitePoints[1] > blackWhitePoints[0])
		? (blackWhitePoints[1] + 1) / (blackWhitePoints[0] + 1)
		: -(blackWhitePoints[0] + 1) / (blackWhitePoints[1] + 1));

	return (blackWhitePoints[1] - blackWhitePoints[0] + relativeStrength) * color;
	//return rand() % 100;
}