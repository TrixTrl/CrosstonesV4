#include "MCTS.h"

std::vector<BoardState::xMove> UCT_Search(BoardState boardState, int searchTime, bool isWhite, std::map<std::string, Node> *tree)
{
    int endTime = time(0) + searchTime;
    std::vector<std::thread *> threads = std::vector<std::thread *>();
    std::mutex treeLock = std::mutex();
    int threadCount = 10;
    for (int i = 0; i < threadCount; i++)
    {
        threads.emplace_back(new std::thread(simulatorLoop, endTime, boardState, isWhite, tree, &treeLock));
    }
    for (int i = 0; i < threadCount; i++)
    {
        threads.at(i)->join();
        delete (threads.at(i));
    }
    // Utils::print("Finished joining threads", true);
    return selectMove(boardState, isWhite, false, 0, 0, tree);
}

void simulatorLoop(int endTime, BoardState boardState, bool isWhite, std::map<std::string, Node> *tree, std::mutex *treeLock)
{
    // Utils::print("Starting thread loop", true);
    while (time(0) < endTime)
    {
        simulate(boardState, isWhite, tree, treeLock);
    }
    return;
}

void simulate(BoardState boardState, bool isWhite, std::map<std::string, Node> *tree, std::mutex *treeLock)
{
    simTreeResult treeResult = simTree(&boardState, isWhite, tree, treeLock);
    bool z = simDefault(boardState, isWhite, treeResult.whiteToPlay);
    // Utils::print("Finished default sim", true);
    treeLock->lock();
    backup(treeResult.states, treeResult.actions, z, treeResult.heuristic, tree);
    treeLock->unlock();
}

simTreeResult simTree(BoardState *boardState, bool isWhite, std::map<std::string, Node> *tree, std::mutex *treeLock)
{
    float c = 0.7;
    float b = 1;
    simTreeResult result = simTreeResult();
    result.whiteToPlay = isWhite;
    bool passed = false;
    // Utils::print(".");
    while (boardState->gameOver(!result.whiteToPlay) == BoardState::winValue::none)
    {
        // Utils::print("!");
        std::string state = boardState->dumpPos() + (result.whiteToPlay ? 'w' : 'b');
        result.states.emplace_back(state);
        treeLock->lock();
        // Utils::print("Locked", true);
        if (tree->find(state) == tree->end())
        {
            newNode(*boardState, result.whiteToPlay, tree);
            treeLock->unlock();
            // Utils::print("Created new node", true);
            std::vector<BoardState::xMove> a = defaultPolicy(*boardState, result.whiteToPlay);
            boardState->makeMove(&a, result.whiteToPlay);
            result.actions.emplace_back(stringify(a));
            result.whiteToPlay = !result.whiteToPlay;
            result.heuristic = staticHeuristic(boardState);
            return result;
        }
        std::vector<BoardState::xMove> a = selectMove(*boardState, isWhite, isWhite != result.whiteToPlay, c, b, tree, &result);
        treeLock->unlock();
        // Utils::print("Unlocked", true);
        result.actions.emplace_back(stringify(a));
        boardState->makeMove(&a, result.whiteToPlay);
        result.whiteToPlay = !result.whiteToPlay;
    }
    // Utils::print(to_string((int)boardState.gameOver(result.whiteToPlay)), true);
    result.heuristic = staticHeuristic(boardState);
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

std::vector<BoardState::xMove> selectMove(BoardState boardState, bool isWhite, bool enemyMove, float c, float b, std::map<std::string, Node> *tree, simTreeResult *result)
{
    std::shared_ptr<std::vector<std::vector<BoardState::xMove>>> legalMoves = boardState.getMoves(enemyMove ? !isWhite : isWhite);
    float aStar = -INFINITY;
    std::vector<BoardState::xMove> bestMove;
    std::map<std::string, Node>::iterator it = tree->find(boardState.dumpPos() + ((isWhite != enemyMove) ? 'w' : 'b'));
    Node *node = &(it->second);
    for (int i = 0; i < legalMoves->size(); i++)
    {
        if (legalMoves->size() > 1 && legalMoves->at(i).size() == 0)
            continue;
        int heuristicStrength = 5;
        bool heuristicActivated = false;
        if (heuristicActivated && node->Qmap.find(stringify(legalMoves->at(i))) == node->Qmap.end())
        {
            // node->Qmap.emplace(stringify(legalMoves->at(i)), heuristic(boardState, legalMoves->at(i)) * (isWhite ? 1 : -1));
            node->Qmap.emplace(stringify(legalMoves->at(i)), enemyMove ? 1 - simpleCaptureHeuristic(legalMoves->at(i)) : simpleCaptureHeuristic(legalMoves->at(i)));
            node->Nmap.emplace(stringify(legalMoves->at(i)), heuristicStrength);
        }
        float evaluation = 0;
        std::map<std::string, float>::iterator qIt = node->Qmap.find(stringify(legalMoves->at(i)));
        std::map<std::string, float>::iterator nIt = node->Nmap.find(stringify(legalMoves->at(i)));
        std::map<std::string, float>::iterator qsIt = node->Qsquigglemap.find(stringify(legalMoves->at(i)));
        if (qIt != node->Qmap.end())
        {
            evaluation += qIt->second * (enemyMove ? -1 : 1);
        }
        else
        {
            evaluation += (enemyMove ? 0 : 1);
        }
        if (nIt != node->Nmap.end())
        {
            evaluation += c * sqrt(log(node->N + 1) / (nIt->second - (heuristicActivated ? heuristicStrength + 1 : -1)));
        }
        else
        {
            evaluation += c * sqrt(log(node->N + 1));
        }
        if (qsIt != node->Qsquigglemap.end())
        {
            evaluation += b * qsIt->second * (enemyMove ? -1 : 1);
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
        Utils::printNode(*node);
    }
    return bestMove;
}

void backup(std::vector<std::string> states, std::vector<std::string> actions, float z, float m, std::map<std::string, Node> *tree)
{
    // Utils::print("Started backup", true);
    for (int i = 0; i < states.size(); i++)
    {
        std::map<std::string, Node>::iterator it = tree->find(states[i]);
        Node *node = &(it->second);
        node->N++;
        std::map<std::string, float>::iterator qIt = node->Qmap.find(actions[i]);
        std::map<std::string, float>::iterator nIt = node->Nmap.find(actions[i]);
        std::map<std::string, float>::iterator qsIt = node->Qsquigglemap.find(actions[i]);
        std::map<std::string, float>::iterator nsIt = node->Nsquigglemap.find(actions[i]);
        if (qIt == node->Qmap.end())
        {
            node->Qmap.emplace(actions[i], 0);
        }
        if (nIt == node->Nmap.end())
        {
            node->Nmap.emplace(actions[i], 0);
        }
        if (qsIt == node->Qsquigglemap.end())
        {
            node->Qsquigglemap.emplace(actions[i], 0);
        }
        if (nsIt == node->Nsquigglemap.end())
        {
            node->Nsquigglemap.emplace(actions[i], 0);
        }
        node->Nmap.at(actions[i]) += 1;
        node->Qmap.at(actions[i]) += (z - node->Qmap.at(actions[i])) / node->Nmap.at(actions[i]);
        node->Nsquigglemap.at(actions[i]) += 1;
        node->Qsquigglemap.at(actions[i]) += (m - node->Qsquigglemap.at(actions[i])) / node->Nsquigglemap.at(actions[i]);
    }
    // Utils::print("Ended backup", true);
}

void newNode(BoardState boardState, bool whiteToPlay, std::map<std::string, Node> *tree)
{
    tree->emplace(boardState.dumpPos() + (whiteToPlay ? 'w' : 'b'), Node());
}

std::vector<BoardState::xMove> defaultPolicy(BoardState boardState, bool whiteToPlay)
{
    std::shared_ptr<std::vector<std::vector<BoardState::xMove>>> legalMoves = boardState.getMoves(whiteToPlay);
    std::vector<int> indices = std::vector<int>();
    for (int i = 0; i < legalMoves->size(); i++)
    {
        indices.emplace_back(i);
    }
    std::shuffle(indices.begin(), indices.end(), std::random_device());
    for (int i = 0; i < legalMoves->size(); i++)
    {
        if (!isLoosing(boardState, whiteToPlay, legalMoves->at(indices[i])))
        {
            return legalMoves->at(indices[i]);
        }
    }
    return std::vector<BoardState::xMove>();
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

bool isLoosing(BoardState boardState, bool whiteToPlay, std::vector<BoardState::xMove> move)
{
    boardState.unsafeMakeMove(&move);
    BoardState::winValue winState = boardState.gameOver(whiteToPlay);
    return winState == (whiteToPlay ? BoardState::winValue::black : BoardState::winValue::white);
}

float heuristic(BoardState boardState, std::vector<BoardState::xMove> move)
{
    boardState.unsafeMakeMove(&move);
    uint8_t board[13][13];
    boardState.copyBoard(&board);
    int whiteScore = 0;
    int blackScore = 0;
    for (int i = 0; i < 13; i++)
    {
        for (int j = 0; j < 13; j++)
        {
            uint8_t piece = board[i][j];
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
    return float(whiteScore) / float(whiteScore + blackScore);
}

float simpleCaptureHeuristic(std::vector<BoardState::xMove> move)
{
    bool whitePartOfMove = false;
    bool blackPartOfMove = false;
    for (auto m : move)
    {
        if (Piece::isTower(m.delta))
        {
            if (Piece::isWhite(m.delta))
            {
                whitePartOfMove = true;
            }
            else
            {
                blackPartOfMove = true;
            }
        }
    }
    float captureDeveation = 0.2;
    return (whitePartOfMove && blackPartOfMove) ? 0.5 + captureDeveation : 0.5 - captureDeveation;
}

float staticHeuristic(BoardState *boardState)
{
    uint8_t board[13][13];
    boardState->copyBoard(&board);
    int whiteScore = 0;
    int blackScore = 0;
    for (int i = 0; i < 13; i++)
    {
        for (int j = 0; j < 13; j++)
        {
            uint8_t piece = board[i][j];
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
    return float(whiteScore) / float(whiteScore + blackScore);
}