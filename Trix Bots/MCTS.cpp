#include "MCTS.h"

std::vector<BoardState::xMove> UCT_Search(BoardState boardState, int searchTime, bool isWhite, std::map<std::string, Node> *tree)
{
    int endTime = time(0) + searchTime;
    int n = 0;
    while (time(0) < endTime)
    {
        // Utils::print(to_string(time(0)), true);
        BoardState simBoard = BoardState();
        simBoard.loadPos(boardState.dumpPos());
        simulate(simBoard, isWhite, tree);
    }
    return selectMove(boardState, isWhite, false, 0, tree);
}

void simulate(BoardState boardState, bool isWhite, std::map<std::string, Node> *tree)
{
    simTreeResult treeResult = simTree(&boardState, isWhite, tree);
    bool z = simDefault(boardState, isWhite, treeResult.whiteToPlay);
    backup(treeResult.states, treeResult.actions, z, tree);
}

simTreeResult simTree(BoardState *boardState, bool isWhite, std::map<std::string, Node> *tree)
{
    float c = 0.9;
    simTreeResult result = simTreeResult();
    result.whiteToPlay = isWhite;
    bool passed = false;
    // Utils::print(".");
    while (boardState->gameOver(!result.whiteToPlay) == BoardState::winValue::none)
    {
        // Utils::print("!");
        std::string state = boardState->dumpPos() + (result.whiteToPlay ? 'w' : 'b');
        result.states.emplace_back(state);
        if (tree->find(state) == tree->end())
        {
            newNode(*boardState, result.whiteToPlay, tree);
            std::vector<BoardState::xMove> a = defaultPolicy(*boardState, result.whiteToPlay);
            boardState->makeMove(&a, result.whiteToPlay);
            result.actions.emplace_back(stringify(a));
            result.whiteToPlay = !result.whiteToPlay;
            return result;
        }
        std::vector<BoardState::xMove> a = selectMove(*boardState, isWhite, isWhite != result.whiteToPlay, c, tree, &result);
        result.actions.emplace_back(stringify(a));
        boardState->makeMove(&a, result.whiteToPlay);
        result.whiteToPlay = !result.whiteToPlay;
    }
    // Utils::print(to_string((int)boardState.gameOver(result.whiteToPlay)), true);
    return result;
}

float simDefault(BoardState boardState, bool isWhite, bool whiteToPlay)
{
    int n = 0;
    while (boardState.gameOver(!whiteToPlay) == BoardState::winValue::none)
    {
        n++;
        std::vector<BoardState::xMove> a = defaultPolicy(boardState, whiteToPlay);
        boardState.makeMove(&a, whiteToPlay);
        whiteToPlay = !whiteToPlay;
    }
    BoardState::winValue winValue = boardState.gameOver(!whiteToPlay);
    float gameResult = winValue == BoardState::winValue::draw ? 0.5 : (winValue == (isWhite ? BoardState::winValue::white : BoardState::winValue::black));
    // Utils::print(to_string(n), false);
    // Utils::print(" | " + to_string(gameResult), true);
    return gameResult;
}

std::vector<BoardState::xMove> selectMove(BoardState boardState, bool isWhite, bool enemyMove, float c, std::map<std::string, Node> *tree, simTreeResult *result)
{
    std::shared_ptr<std::vector<std::vector<BoardState::xMove>>> legalMoves = boardState.getMoves(enemyMove ? !isWhite : isWhite);
    float aStar = -INFINITY;
    std::vector<BoardState::xMove> bestMove;
    std::map<std::string, Node>::iterator it = tree->find(boardState.dumpPos() + ((isWhite != enemyMove) ? 'w' : 'b'));
    Node node = it->second;
    for (int i = 0; i < legalMoves->size(); i++)
    {
        if (legalMoves->size() > 1 && legalMoves->at(i).size() == 0)
            continue;
        float evaluation = 0;
        std::map<std::string, float>::iterator qIt = node.Qmap.find(stringify(legalMoves->at(i)));
        std::map<std::string, float>::iterator nIt = node.Nmap.find(stringify(legalMoves->at(i)));
        if (qIt != node.Qmap.end())
        {
            evaluation += qIt->second * (enemyMove ? -1 : 1);
        }
        else
        {
            evaluation += (enemyMove ? 0 : 1);
        }
        if (nIt != node.Nmap.end())
        {
            evaluation += c * sqrt(log(node.N + 1) / nIt->second);
        }
        else
        {
            evaluation += c * sqrt(log(node.N + 1));
        }
        std::string move = stringify(legalMoves->at(i));
        if (evaluation > aStar && (result == nullptr || legalMoves->at(i).size() == 0 || find(result->actions.begin(), result->actions.end(), move) == result->actions.end()))
        {
            aStar = evaluation;
            bestMove = legalMoves->at(i);
        }
    }
    if (c == 0)
    {
        Utils::print(std::to_string(aStar), true);
        Utils::printNode(node);
    }
    return bestMove;
}

void backup(std::vector<std::string> states, std::vector<std::string> actions, float z, std::map<std::string, Node> *tree)
{
    for (int i = 0; i < states.size(); i++)
    {
        std::map<std::string, Node>::iterator it = tree->find(states[i]);
        Node *node = &(it->second);
        node->N++;
        std::map<std::string, float>::iterator qIt = node->Qmap.find(actions[i]);
        std::map<std::string, float>::iterator nIt = node->Nmap.find(actions[i]);
        if (qIt == node->Qmap.end())
        {
            node->Qmap.emplace(actions[i], 0);
        }
        if (nIt == node->Nmap.end())
        {
            node->Nmap.emplace(actions[i], 0);
        }
        node->Nmap.at(actions[i]) += 1;
        node->Qmap.at(actions[i]) += (z - node->Qmap.at(actions[i])) / node->Nmap.at(actions[i]);
    }
}

void newNode(BoardState boardState, bool whiteToPlay, std::map<std::string, Node> *tree)
{
    tree->emplace(boardState.dumpPos() + (whiteToPlay ? 'w' : 'b'), Node());
}

std::vector<BoardState::xMove> defaultPolicy(BoardState boardState, bool whiteToPlay)
{
    std::shared_ptr<std::vector<std::vector<BoardState::xMove>>> legalMoves = boardState.getMoves(whiteToPlay);
    return legalMoves->at(rand() % legalMoves->size());
}

std::string stringify(std::vector<BoardState::xMove> move)
{
    std::string output = "";
    for (int i = 0; i < move.size(); i++)
    {
        output += std::to_string(move[i].delta) + "|" + std::to_string(move[i].i) + "|" + std::to_string(move[i].j) + "|";
    }
    return output;
}