#include "Evaluation.h"

float Evaluator::basicEvaluation(const uint8_t (*pieces)[13][13])
{
    float blackScore = 0;
    float whiteScore = 0;

    for (int x = 0; x < 13; x++)
    {
        for (int y = 0; y < 13; y++)
        {
            uint8_t piece = (*pieces)[x][y];
            if (Piece::isTower(piece))
            {
                if (Piece::isWhite(piece))
                {
                    whiteScore += Piece::height(piece);
                }
                else
                {
                    blackScore += Piece::height(piece);
                }
            }
        }
    }
    return whiteScore - blackScore;
}