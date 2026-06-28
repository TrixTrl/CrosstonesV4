#include "AlphaCruncher.h"
#include "MCTS.h"

void AlphaCruncher::getMoveToPlay(uint8_t (*board)[13][13], bool isWhite, int endTime)
{
    BoardState_T boardState = BoardState_T();
    boardState.loadPos(Utils::convertToPosString(board));
    Utils::print("Tree size before pruning: " + std::to_string(tree.size()), true);
    std::vector<std::string> toRemove = std::vector<std::string>();
    toRemove.reserve(tree.size());
    for (auto const &n : tree)
    {
        if (!tree.at(n.first).recentlyVisited)
        {
            // tree.erase(n.first);
            toRemove.emplace_back(n.first);
        }
        else
        {
            tree.at(n.first).recentlyVisited = false;
        }
    }
    for (const auto position : toRemove)
    {
        tree.erase(position);
    }
    Utils::print("Tree size after pruning: " + std::to_string(tree.size()), true);
    std::vector<BoardState_T::xMove> bestMove = UCT_Search(boardState, thinkingTime, isWhite, &tree);
    Utils::printNode(tree.at(Utils::convertToPosString(board) + (isWhite ? 'w' : 'b')));
    Utils::print(stringify(bestMove), true);
    Utils::print(std::to_string(tree.size()));
    for (int i = 0; i < bestMove.size(); i++)
    {
        (*board)[bestMove[i].i][bestMove[i].j] ^= bestMove[i].delta;
    }

    Utils::print("", true);
    Utils::printNode(tree.at(Utils::convertToPosString(board) + (isWhite ? 'b' : 'w')));
}