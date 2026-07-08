#include "DampSock.h"

void DampSock::getMoveToPlay(uint8_t (*board)[13][13], bool isWhite, int endTime)
{
    std::map<std::string, DampNode>::iterator it = table.find(Utils::getKey(board) + (isWhite ? 'w' : 'b'));
    int searchDepth = 0;
    if (it != table.end())
    {
        searchDepth = it->second.depth + 1;
    }
    int endOfThinkingTime = Utils::millis() + thinkingTime * 1000;
    while (true)
    {
        float positionEval = alphaBeta_sockVersion(board, searchDepth, -INFINITY, INFINITY, isWhite, table, endOfThinkingTime).second;
        Utils::print(positionEval, true);
        Utils::print(searchDepth, true);
        searchDepth++;
        if (Utils::millis() >= endOfThinkingTime)
            break;
    }

    Utils::print("Table size: " + std::to_string(table.size()), true);

    int bestMove = 0;
    float bestEval = isWhite ? -INFINITY : INFINITY;
    std::vector<FastMoveGenerator::move> moves = FastMoveGenerator::getMoves(board, isWhite);
    int index = 0;
    for (auto &move : moves)
    {
        uint8_t nextPieces[13][13];
        memcpy(&nextPieces, board, sizeof(*board));
        FastMoveGenerator::applyMove(&nextPieces, move);
        std::string positionKey = Utils::getKey(&nextPieces) + (isWhite ? 'b' : 'w');
        std::map<std::string, DampNode>::iterator it = table.find(positionKey);
        if (it == table.end())
        {
            throw;
        }
        float eval = it->second.evaluation;
        if (isWhite)
        {
            if (eval > bestEval)
            {
                bestEval = eval;
                bestMove = index;
            }
        }
        else
        {
            if (eval < bestEval)
            {
                bestEval = eval;
                bestMove = index;
            }
        }
        index++;
    }

    FastMoveGenerator::applyMove(board, moves[bestMove]);
}