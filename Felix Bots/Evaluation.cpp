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

	auto winValue = Utility::calcWinValue(&board.square, board.isWhiteTurn);
	if (winValue != GameResult::InProgress) {
		switch (winValue)
		{
		case GameResult::WhiteHasWon:
			return Searcher::positiveInfinity * color;
		case GameResult::BlackHasWon:
			return Searcher::negativeInfinity * color;
		case GameResult::Draw:
			return 0;
		}
	}
	const int pieceWeight = 100;

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
				blackWhitePoints[blackWhiteIndex] += pieceVal * pieceWeight 
					+ pieceVal * positionMap[x][y] * positionWeight;
			}
		}
	}
	int relativeStrength = 5 * ((blackWhitePoints[1] > blackWhitePoints[0])
		? (blackWhitePoints[1] + 1) / (blackWhitePoints[0] + 1)
		: -(blackWhitePoints[0] + 1) / (blackWhitePoints[1] + 1));

	return (blackWhitePoints[1] - blackWhitePoints[0] + relativeStrength) * color;
	//return rand() % 100;
}