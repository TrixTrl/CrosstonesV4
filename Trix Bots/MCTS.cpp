#include "MCTS.h"

vector<BoardState::xMove> UCT_Search(BoardState boardState, int searchTime, bool isWhite, map<string, Node> *tree)
{
    int endTime = time(0) + searchTime;
    while (time(0) < endTime)
    {
        BoardState simBoard = BoardState();
        simBoard.loadPos(boardState.dumpPos());
        simulate(simBoard, isWhite, tree);
    }
    return selectMove(boardState, isWhite, false, 0, tree);
}

void simulate(BoardState boardState, bool isWhite, map<string, Node> *tree)
{
    simTreeResult treeResult = simTree(boardState, isWhite, tree);
    bool z = simDefault(boardState, isWhite, treeResult.whiteToPlay);
    backup(treeResult.states, treeResult.actions, z, tree);
}

simTreeResult simTree(BoardState boardState, bool isWhite, map<string, Node> *tree)
{
    float c = 0.8;
    simTreeResult result = simTreeResult();
    result.whiteToPlay = isWhite;
    while (boardState.gameOver(result.whiteToPlay) == BoardState::winValue::none)
    {
        string state = boardState.dumpPos() + (result.whiteToPlay ? 'w' : 'b');
        result.states.emplace_back(state);
        if (tree->find(state) == tree->end())
        {
            newNode(boardState, result.whiteToPlay, tree);

            return result;
        }
        vector<BoardState::xMove> a = selectMove(boardState, isWhite, isWhite ^ result.whiteToPlay, c, tree);
        boardState.makeMove(&a, result.whiteToPlay);
        result.whiteToPlay = !result.whiteToPlay;
    }
    return result;
}

float simDefault(BoardState boardState, bool isWhite, bool whiteToPlay)
{
    while (boardState.gameOver(whiteToPlay) == BoardState::winValue::none)
    {
        vector<BoardState::xMove> a = defaultPolicy(boardState, whiteToPlay);
        boardState.makeMove(&a, whiteToPlay);
        whiteToPlay = whiteToPlay;
    }
    return boardState.gameOver(whiteToPlay) == BoardState::winValue::draw ? 0.5 : boardState.gameOver(whiteToPlay) == (isWhite ? BoardState::winValue::black : BoardState::winValue::white);
}

vector<BoardState::xMove> selectMove(BoardState boardState, bool isWhite, bool enemyMove, float c, map<string, Node> *tree)
{
    std::shared_ptr<std::vector<std::vector<BoardState::xMove>>> legalMoves = boardState.getMoves(enemyMove ? !isWhite : isWhite);
    float aStar = -INFINITY;
    std::vector<BoardState::xMove> bestMove;
    std::map<std::string, Node>::iterator it = tree->find(boardState.dumpPos() + ((isWhite ^ enemyMove) ? 'w' : 'b'));
    Node node = it->second;
    for (int i = 0; i < legalMoves->size(); i++)
    {
        float evaluation = 0;
        std::map<std::string, float>::iterator qIt = node.Qmap.find(stringify(legalMoves->at(i)));
        std::map<std::string, float>::iterator nIt = node.Nmap.find(stringify(legalMoves->at(i)));
        if (qIt != node.Qmap.end())
        {
            evaluation += qIt->second;
        }
        if (nIt != node.Qmap.end())
        {
            evaluation += c * sqrt(log(node.N) / nIt->second);
        }
        if (evaluation > aStar)
        {
            aStar = evaluation;
            bestMove = legalMoves->at(i);
        }
    }
    return bestMove;
}

void backup(vector<string> states, vector<string> actions, float z, map<string, Node> *tree)
{
    for (int i = 0; i < states.size(); i++)
    {
        std::map<std::string, Node>::iterator it = tree->find(states[i]);
        Node node = it->second;
        node.N++;
        std::map<std::string, float>::iterator qIt = node.Qmap.find(actions[i]);
        std::map<std::string, float>::iterator nIt = node.Nmap.find(actions[i]);
        float qValue = 0;
        float nValue = 0;
        if (qIt != node.Qmap.end())
        {
            qValue = qIt->second;
        }
        else
        {
            node.Qmap.emplace(actions[i], 0);
        }
        if (nIt != node.Qmap.end())
        {
            nValue = nIt->second;
        }
        else
        {
            node.Nmap.emplace(actions[i], 0);
        }
        node.Nmap.at(actions[i]) += 1;
        node.Qmap.at(actions[i]) += (z - node.Qmap.at(actions[i])) / node.Nmap.at(actions[i]);
    }
}

void newNode(BoardState boardState, bool whiteToPlay, map<string, Node> *tree)
{
    tree->emplace(boardState.dumpPos() + (whiteToPlay ? 'w' : 'b'), Node());
}

vector<BoardState::xMove> defaultPolicy(BoardState boardState, bool whiteToPlay)
{
    std::shared_ptr<std::vector<std::vector<BoardState::xMove>>> legalMoves = boardState.getMoves(whiteToPlay);
    return legalMoves->at(rand() % legalMoves->size());
}

string stringify(vector<BoardState::xMove> move)
{
    string output = "";
    for (int i = 0; i < move.size(); i++)
    {
        output += to_string(move[i].delta) + "|" + to_string(move[i].i) + "|" + to_string(move[i].j);
    }
    return output;
}