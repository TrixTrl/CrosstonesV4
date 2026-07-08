#include "Evaluation.h"

float Evaluator::basicEvaluation(const uint8_t (*pieces)[13][13])
{
    const int boardScores[13][13] = {
        {1, 1, 1, 1, 4, 5, 5, 5, 4, 1, 1, 1, 1},
        {1, 0, 1, 0, 4, 0, 5, 0, 4, 0, 1, 0, 1},
        {1, 1, 1, 1, 4, 5, 5, 5, 4, 1, 1, 1, 1},
        {1, 0, 1, 0, 4, 0, 5, 0, 4, 0, 1, 0, 1},
        {1, 1, 1, 1, 4, 4, 4, 4, 4, 1, 1, 1, 1},
        {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1},
        {4, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 4},
        {3, 0, 1, 0, 1, 0, 3, 0, 1, 0, 1, 0, 3},
        {3, 3, 2, 2, 2, 3, 3, 3, 2, 2, 2, 3, 3},
        {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};

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
                    whiteScore += log(Piece::height(piece) * boardScores[y][x] + 1);
                }
                else
                {
                    blackScore += log(Piece::height(piece) * boardScores[12 - y][x] + 1);
                }
            }
        }
    }
    return whiteScore - blackScore;
}