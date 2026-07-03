#include "felix-bots/evaluation/Evaluation.h"
#include "felix-bots/search/Searcher.h"
#include "felix-bots/board/GameResult.h"
#include "felix-bots/util/Utility.h"
#include "globals/Piece.h"

using namespace dc;

const int Evaluation::positionMap[13][13] =
{
	1, 1, 2, 3, 2, 3, 4, 3, 2, 3, 2, 1, 1,
	1, 0, 1, 0, 2, 0, 4, 0, 2, 0, 1, 0, 1,
	2, 1, 3, 3, 2, 2, 5, 2, 2, 3, 3, 1, 2,
	2, 0, 2, 0, 4, 0, 3, 0, 4, 0, 2, 0, 1,
	3, 3, 3, 3, 5, 5, 5, 5, 5, 3, 3, 3, 3,
	3, 0, 2, 0, 3, 0, 5, 0, 3, 0, 2, 0, 3,
	3, 2, 3, 3, 4, 3, 5, 3, 4, 3, 3, 2, 3,
	3, 0, 2, 0, 3, 0, 5, 0, 3, 0, 2, 0, 3,
	3, 3, 3, 3, 5, 4, 5, 4, 5, 3, 3, 3, 3,
	2, 0, 2, 0, 3, 0, 3, 0, 3, 0, 2, 0, 1,
	2, 1, 3, 3, 2, 2, 5, 2, 2, 3, 3, 1, 2,
	1, 0, 1, 0, 2, 0, 3, 0, 2, 0, 1, 0, 1,
	1, 1, 2, 3, 2, 3, 4, 3, 2, 3, 2, 1, 1
};

const int Evaluation::pieceValue[22] =
{
	0, //none
	7, 20, 33, 37, 46, //normal towers 
	0, 0, 0,
	0, 30, 40, 45, 55, //blue towers
	0, 0, 0,
	0, 43, 66, 70, 80 //red towrs
};


// The score that's returned is given from the perspective of whoever's turn it is to move.
// So a positive score means the player who's turn it is to move has an advantage, while a negative score indicates a disadvantage.
int Evaluation::evaluate(const BotBoard& board)
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

	int materialPoints[2] = { 0, 0 }; // 0: points for black; 1: points for white
	int positionalPoints[2] = { 0, 0 };
	int weightedMatPoints[2] = { 0, 0 };

	for (int x = 0; x < 13; x++)
	{
		for (int y = 0; y < 13; y++)
		{
			uint8_t content = board.square[x][y];
			if (Piece::isTower(content))
			{
				const int blackWhiteIndex = Piece::isWhite(content);
				const int pieceColorFactor = 2 * blackWhiteIndex - 1; //1 for white, -1 for black

				int pieceVal = (Piece::height(content)
					+ (Piece::isBlue(content) ? 1 : 0)
					+ (Piece::isRed(content) ? 3 : 0));

				materialPoints[blackWhiteIndex] += pieceVal;
				weightedMatPoints[blackWhiteIndex] += pieceValue[content & 0b00011111];

				positionalPoints[blackWhiteIndex] += (pieceValue[content & 0b00011111] + pieceVal)
					* positionMap[(1 - blackWhiteIndex) * 12 + pieceColorFactor * y][x];
			}
		}
	}

	int relativeMatStrength = ((materialPoints[1] > materialPoints[0])
			? (materialPoints[1] + 1) * 100 / (materialPoints[0] + 1)
			: -(materialPoints[0] + 1)  * 100 / (materialPoints[1] + 1));

	for (int i = 0; i < relativeExp; i++)
		relativeMatStrength = relativeMatStrength * abs(relativeMatStrength) / 100; //make quadratic
	relativeMatStrength = relativeMatStrength * relativeWeight / 100;

	for (int i = 0; i <= 1; i++)
	{
		materialPoints[i] *= materialWeight;
		weightedMatPoints[i] *= weightedMatWeight;
		positionalPoints[i] *= positionWeight;
	}

	return (materialPoints[1] + weightedMatPoints[1] + positionalPoints[1]
		- materialPoints[0] - weightedMatPoints[0] - positionalPoints[0]
		+ relativeMatStrength) * color;
	//return rand() % 100;
}

int Evaluation::getEffectivePieceWorth(const BotBoard& board, const uint8_t piece, const int x, const int y) const
{
	const int pieceVal = (Piece::height(piece)
		+ Piece::isBlue(piece)
		+ 3 * Piece::isRed(piece));

	const int weightedPieceVal = pieceValue[piece & 0b00011111];
	const int blackWhiteIndex = Piece::isWhite(piece);
	const int pieceColorFactor = 2 * blackWhiteIndex - 1;

	const int positionVal = positionMap[(1 - blackWhiteIndex) * 12 + pieceColorFactor * y][x];

	return pieceVal * materialWeight

		+ weightedPieceVal * weightedMatWeight
		+ (pieceVal + weightedPieceVal) * positionVal * positionWeight;
}