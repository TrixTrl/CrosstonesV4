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
}