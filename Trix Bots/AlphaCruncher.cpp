#include "AlphaCruncher.h"
#include "MCTS.h"

void AlphaCruncher::getMoveToPlay(uint8_t (*board)[13][13], bool isWhite, int endTime)
{
    BoardState boardState = BoardState();
    boardState.loadPos(Utils::convertToPosString(board));

    std::vector<BoardState::xMove> bestMove = UCT_Search(boardState, thinkingTime, isWhite, &tree);
    Utils::print(stringify(bestMove), true);
    Utils::print(to_string(tree.size()));
    for (int i = 0; i < bestMove.size(); i++)
    {
        (*board)[bestMove[i].i][bestMove[i].j] ^= bestMove[i].delta;
    }
}