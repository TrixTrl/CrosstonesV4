#include "AlphaBeta.h"

// #define PRINTPOSITIONSINSEARCH
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

std::pair<bool, float> alphaBeta_sockVersion(uint8_t (*pieces)[13][13], int depth, float alpha, float beta, bool maximizing, std::map<std::string, DampNode> &table, int endOfThinkingTime)
{
    if (Utils::millis() >= endOfThinkingTime)
        return std::pair<bool, float>(false, 0);
    std::string positionKey = Utils::getKey(pieces) + (maximizing ? 'w' : 'b');
    std::map<std::string, DampNode>::iterator it = table.find(positionKey);
    DampNode *node;
    if (it != table.end())
    {
        node = &(it->second);
    }
    else if (depth != 0)
    {
        // throw;
    }
    if (depth == 0)
    {
        float evaluation = Evaluator::basicEvaluation(pieces);
        table.emplace(positionKey, DampNode(evaluation, 0));
        return std::pair<bool, float>(true, evaluation);
    }
    BoardState_T::winValue winValue = BoardState_T(pieces).gameOver(!maximizing);
    if (winValue != BoardState_T::winValue::none)
    {
        if (winValue == BoardState_T::winValue::white)
        {
            table.emplace(positionKey, DampNode(INFINITY, 1000000));
            return std::pair<bool, float>(true, INFINITY);
        }
        if (winValue == BoardState_T::winValue::black)
        {
            table.emplace(positionKey, DampNode(-INFINITY, 1000000));
            return std::pair<bool, float>(true, -INFINITY);
        }
        table.emplace(positionKey, DampNode(0, 1000000));
        return std::pair<bool, float>(true, 0);
    }
    if (maximizing)
    {
        float value = -INFINITY;

#ifdef PRINTPOSITIONSINSEARCH
        Utils::print(BoardState_T(pieces).dumpPos(), true);
        Utils::print(maximizing, true);
#endif
        std::vector<FastMoveGenerator::move> moves = FastMoveGenerator::getMoves(pieces, true);

        for (const auto &move : moves)
        {
            uint8_t nextPieces[13][13];
            memcpy(&nextPieces, pieces, sizeof(*pieces));
            FastMoveGenerator::applyMove(&nextPieces, move);
            std::pair<bool, float> searchResult = alphaBeta_sockVersion(&nextPieces, depth - 1, alpha, beta, false, table, endOfThinkingTime);
            if (!searchResult.first)
                return searchResult;
            value = max(value, searchResult.second);
            if (value >= beta)
            {
                break;
            }
            alpha = max(alpha, value);
        }
        table.emplace(positionKey, DampNode(value, depth));
        return std::pair<bool, float>(true, value);
    }
    else
    {
        float value = INFINITY;

#ifdef PRINTPOSITIONSINSEARCH
        Utils::print(BoardState_T(pieces).dumpPos(), true);
        Utils::print(maximizing, true);
#endif
        std::vector<FastMoveGenerator::move> moves = FastMoveGenerator::getMoves(pieces, false);

        for (const auto &move : moves)
        {
            uint8_t nextPieces[13][13];
            memcpy(&nextPieces, pieces, sizeof(*pieces));
            FastMoveGenerator::applyMove(&nextPieces, move);
            std::pair<bool, float> searchResult = alphaBeta_sockVersion(&nextPieces, depth - 1, alpha, beta, true, table, endOfThinkingTime);
            if (!searchResult.first)
                return searchResult;
            value = min(value, searchResult.second);
            if (value <= alpha)
            {
                break;
            }
            beta = min(beta, value);
        }
        table.emplace(positionKey, DampNode(value, depth));
        return std::pair<bool, float>(true, value);
    }
}