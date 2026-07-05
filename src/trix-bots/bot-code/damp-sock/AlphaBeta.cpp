#include "AlphaBeta.h"

/*
function alphabeta(node, depth, α, β, maximizingPlayer) is
    if depth == 0 or node is terminal then
        return the heuristic value of node
    if maximizingPlayer then
        value := −∞
        for each child of node do
            value := max(value, alphabeta(child, depth − 1, α, β, FALSE))
            if value ≥ β then
                break (* β cutoff *)
            α := max(α, value)
        return value
    else
        value := +∞
        for each child of node do
            value := min(value, alphabeta(child, depth − 1, α, β, TRUE))
            if value ≤ α then
                break (* α cutoff *)
            β := min(β, value)
        return value
*/

float alphaBeta_sockVersion(uint8_t (*pieces)[13][13], int depth, float alpha, float beta, bool maximizing)
{
    if (depth == 0)
    {
        return Evaluator::basicEvaluation(pieces);
    }
    BoardState_T::winValue winValue = BoardState_T(pieces).gameOver(!maximizing);
    if (winValue != BoardState_T::winValue::none)
    {
        if (winValue == BoardState_T::winValue::white)
        {
            return INFINITY;
        }
        if (winValue == BoardState_T::winValue::black)
        {
            return -INFINITY;
        }
        return 0;
    }
    if (maximizing)
    {
        float value = -INFINITY;

        std::vector<FastMoveGenerator::move> moves = FastMoveGenerator::getMoves(pieces, true);

        for (const auto &move : moves)
        {
            uint8_t nextPieces[13][13];
            memcpy(&nextPieces, pieces, sizeof(*pieces));
            FastMoveGenerator::applyMove(&nextPieces, move);
            value = max(value, alphaBeta_sockVersion(&nextPieces, depth - 1, alpha, beta, false));
            if (value >= beta)
            {
                break;
            }
            alpha = max(alpha, value);
        }
        return value;
    }
    else
    {
        float value = INFINITY;

        std::vector<FastMoveGenerator::move> moves = FastMoveGenerator::getMoves(pieces, false);

        for (const auto &move : moves)
        {
            uint8_t nextPieces[13][13];
            memcpy(&nextPieces, pieces, sizeof(*pieces));
            FastMoveGenerator::applyMove(&nextPieces, move);
            value = min(value, alphaBeta_sockVersion(&nextPieces, depth - 1, alpha, beta, true));
            if (value <= alpha)
            {
                break;
            }
            beta = min(beta, value);
        }
        return value;
    }
}