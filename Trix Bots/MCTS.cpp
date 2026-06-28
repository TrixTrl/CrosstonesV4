#include "MCTS.h"

std::vector<BoardState_T::xMove> UCT_Search(BoardState_T boardState, int searchTime, bool isWhite, std::map<std::string, Node> *tree)
{
    int endTime = time(0) + searchTime;
    std::vector<std::thread *> threads = std::vector<std::thread *>();
    std::shared_mutex treeLock = std::shared_mutex();
    int threadCount = 16;
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
    return selectMove(boardState, isWhite, false, 0, tree);
}

void simulatorLoop(int endTime, BoardState_T boardState, bool isWhite, std::map<std::string, Node> *tree, std::shared_mutex *treeLock)
{
    // Utils::print("Starting thread loop", true);
    while (time(0) < endTime)
    {
        simulate(boardState, isWhite, tree, treeLock);
    }
    return;
}

void simulate(BoardState_T boardState, bool isWhite, std::map<std::string, Node> *tree, std::shared_mutex *treeLock)
{
    simTreeResult treeResult = simTree(&boardState, isWhite, tree, treeLock);
    treeResult.z = simDefault(boardState, isWhite, treeResult.whiteToPlay);
#ifdef KILLER_MOVES_ACTIVATED
    if (treeResult.killerMoves.size() > 0)
    {
        // Utils::print(std::to_string(treeResult.killerMoves.size()), true);
        treeLock->lock();
        spreadKillerMoves(treeResult.killerMoves, tree);
        treeLock->unlock();
    }
#endif
    // Utils::print("Finished default sim", true);
    backup(treeResult.states, treeResult.actions, treeResult.z, treeResult.heuristic, tree, treeLock);
}

simTreeResult simTree(BoardState_T *boardState, bool isWhite, std::map<std::string, Node> *tree, std::shared_mutex *treeLock)
{
    float c = 0.3;
    simTreeResult result = simTreeResult();
    result.whiteToPlay = isWhite;
    bool passed = false;
    // Utils::print(".");
    while (boardState->gameOver(!result.whiteToPlay) == BoardState_T::winValue::none)
    {
        // Utils::print("!");
        std::string state = boardState->dumpPos() + (result.whiteToPlay ? 'w' : 'b');
        result.states.emplace_back(state);
        treeLock->lock_shared();
        // Utils::print("Locked", true);
        if (tree->find(state) == tree->end())
        {
            treeLock->unlock_shared();
            newNode(*boardState, result.whiteToPlay, isWhite, tree, treeLock);
            std::vector<BoardState_T::xMove> a = defaultPolicy(*boardState, result.whiteToPlay, false);
            boardState->unsafeMakeMove(&a);
            result.actions.emplace_back(stringify(a));
            result.whiteToPlay = !result.whiteToPlay;
#ifdef BOARD_HEURISTIC_ACTIVATED
            // result.heuristic = stolenHeuristic(*boardState, std::vector<BoardState_T::xMove>(), /*result.whiteToPlay*/ isWhite);
            result.heuristic = simpleHeuristic(boardState, isWhite);
#endif
            return result;
        }
        std::vector<BoardState_T::xMove> a = selectMove(*boardState, isWhite, isWhite != result.whiteToPlay, c, tree, &result, treeLock);
        // Utils::print("Unlocked", true);
        result.actions.emplace_back(stringify(a));
        boardState->unsafeMakeMove(&a);
        result.whiteToPlay = !result.whiteToPlay;
    }
// Utils::print(to_string((int)boardState.gameOver(result.whiteToPlay)), true);
// result.heuristic = staticHeuristic(boardState) * (isWhite ? 1 : -1);
#ifdef BOARD_HEURISTIC_ACTIVATED
    // result.heuristic = stolenHeuristic(*boardState, std::vector<BoardState_T::xMove>(), /*result.whiteToPlay*/ isWhite);
    result.heuristic = simpleHeuristic(boardState, isWhite);
#endif
    return result;
}

float simDefault(BoardState_T boardState, bool isWhite, bool whiteToPlay)
{
    int n = 0;
    std::pair<bool, bool> passes = std::pair<bool, bool>(false, false);
    while (boardState.gameOver(!whiteToPlay) == BoardState_T::winValue::none)
    {
        n++;
        std::vector<BoardState_T::xMove> a = defaultPolicy(boardState, whiteToPlay, true);
        boardState.unsafeMakeMove(&a);
        if (a.size() == 0)
        {
            if (whiteToPlay)
            {
                if (passes.first)
                    return (isWhite ? -1 : 1) / (log(n + 1) + 1);
                passes.first = true;
            }
            else
            {
                if (passes.second)
                    return (isWhite ? 1 : -1) / (log(n + 1) + 1);
                passes.second = true;
            }
        }
        else
        {
            if (whiteToPlay)
            {
                passes.first = false;
            }
            else
            {
                passes.second = false;
            }
        }
        whiteToPlay = !whiteToPlay;
    }
    BoardState_T::winValue winValue = boardState.gameOver(!whiteToPlay);
    float gameResult = (winValue == BoardState_T::winValue::draw) ? 0 : ((winValue == (isWhite ? BoardState_T::winValue::white : BoardState_T::winValue::black)) ? 1 : -1);
    // Utils::print(to_string(n), false);
    // Utils::print("Game Result: " + std::to_string(gameResult), true);
    return gameResult / (log(n + 1) + 1);
}

std::vector<BoardState_T::xMove> selectMove(BoardState_T boardState, bool isWhite, bool enemyMove, float c, std::map<std::string, Node> *tree, simTreeResult *result, std::shared_mutex *treeLock)
{
    std::shared_ptr<std::vector<std::vector<BoardState_T::xMove>>> legalMoves = boardState.getMoves(enemyMove ? !isWhite : isWhite, false, false);
    float aStar = -INFINITY;
    float aStarClean = 0;
    std::vector<BoardState_T::xMove> bestMove;
    std::string position = boardState.dumpPos() + ((isWhite != enemyMove) ? 'w' : 'b');
    std::map<std::string, Node>::iterator it = tree->find(position);
    Node *node = &(it->second);
#ifdef MOVE_HEURISTIC_ACTIVATED
    bool locked = false;
    if (node->N == 0)
    {
        if (treeLock != nullptr)
        {
            treeLock->unlock_shared();
            treeLock->lock();
            std::map<std::string, Node>::iterator n2it = tree->find(position);
            node = &(n2it->second);
            locked = true;
        }
    }
#endif
    float totalEvaluation = 0;
    for (int i = 0; i < legalMoves->size(); i++)
    {
        if (legalMoves->size() > 1 && legalMoves->at(i).size() == 0)
            continue;
        std::string move = stringify(legalMoves->at(i));
#ifdef MOVE_HEURISTIC_ACTIVATED
        int heuristicStrength = 5;
        if (node->N == 0)
        {
            if (node->Qmap.find(move) == node->Qmap.end())
            {
                // node->Qmap.emplace(move, heuristic(boardState, legalMoves->at(i)) * (isWhite ? 1 : -1));
                // node->Qmap.emplace(move, stolenHeuristic(boardState, legalMoves->at(i), enemyMove ? !isWhite : isWhite) * (enemyMove ? -1 : 1));
                BoardState_T boardStateCopy = BoardState_T();
                boardStateCopy.loadPos(boardState.dumpPos());
                boardStateCopy.unsafeMakeMove(&legalMoves->at(i));
                node->Qmap.emplace(move, simpleHeuristic(&boardStateCopy, isWhite) * 0.01);
                node->Nmap.emplace(move, heuristicStrength);
            }
        }
#endif
        float evaluation = 0;
        float cleanEvaluation = 0;
        std::map<std::string, float>::iterator qIt = node->Qmap.find(move);
        std::map<std::string, float>::iterator nIt = node->Nmap.find(move);
#ifdef BOARD_HEURISTIC_ACTIVATED
        std::map<std::string, float>::iterator qsIt = node->Qsquigglemap.find(move);
        // std::map<std::string, float>::iterator nsIt = node->Nsquigglemap.find(move);
#endif
#ifdef KILLER_MOVES_ACTIVATED
        std::map<std::string, float>::iterator kIt = node->Kmap.find(move);
#endif
        if (qIt != node->Qmap.end())
        {
            evaluation += qIt->second * (enemyMove ? -1 : 1);
        }
        if (nIt != node->Nmap.end())
        {
#ifdef BOARD_HEURISTIC_ACTIVATED
            if (qsIt != node->Qsquigglemap.end())
            {
                // float b = c > 0 ? 2 / sqrt(nIt->second) : 0.5;
                float b = 0.5;
                evaluation += b * (qsIt->second - node->nodeQsquiggle) / (log(nIt->second + 1) + 1) * (enemyMove ? -1 : 1);
            }
#endif
            totalEvaluation += evaluation;
            cleanEvaluation = evaluation;
#ifdef MOVE_HEURISTIC_ACTIVATED
            evaluation += c * sqrt(log(node->N + 1) / (nIt->second - (heuristicStrength - 1)));
#else
            evaluation += c * sqrt(log(node->N + 1) / (nIt->second + 1));
#endif
#ifdef KILLER_MOVES_ACTIVATED
            if (kIt != node->Kmap.end() && c > 0)
            {
                float b = 2;
                evaluation += b * log(kIt->second) / nIt->second;
                // Utils::print(std::to_string(evaluation) + " | " + std::to_string(10 / log(kIt->second) / log(node->N + 1)), true);
            }
#endif
        }
        else
        {
            evaluation += 10000000; // c * sqrt(log(node->N + 1));
        }
        if (evaluation > aStar && (result == nullptr || legalMoves->at(i).size() == 0 || find(result->actions.begin(), result->actions.end(), move) == result->actions.end()))
        {
            aStar = evaluation;
            bestMove = legalMoves->at(i);
            aStarClean = cleanEvaluation;
        }
    }
    if (c == 0)
    {
        Utils::print(std::to_string(aStar), true);
        // Utils::printNode(*node);
    }
#ifdef KILLER_MOVES_ACTIVATED
    float killerMove = aStarClean - totalEvaluation / node->Nmap.size();
    if (killerMove > 5 && result != nullptr)
    {
        result->killerMoves.emplace_back(stringify(bestMove));
    }
#endif
#ifdef MOVE_HEURISTIC_ACTIVATED
    if (locked)
    {
        treeLock->unlock();
    }
    else
    {
        if (treeLock != nullptr)
        {
            treeLock->unlock_shared();
        }
    }
#else
    if (treeLock != nullptr)
    {
        treeLock->unlock_shared();
    }
#endif
    return bestMove;
}

void backup(std::vector<std::string> states, std::vector<std::string> actions, float z, float m, std::map<std::string, Node> *tree, std::shared_mutex *treeLock)
{
    // Utils::print("Started backup", true);
    if (states.size() == 0)
        return;
    treeLock->lock();
    std::map<std::string, Node>::iterator rIt = tree->find(states[0]);
    Node *rootNode = &(rIt->second);
    for (int i = 0; i < states.size(); i++)
    {
        std::map<std::string, Node>::iterator it = tree->find(states[i]);
        Node *node = &(it->second);
        node->N++;
        node->recentlyVisited = true;
        std::map<std::string, float>::iterator qIt = node->Qmap.find(actions[i]);
        std::map<std::string, float>::iterator nIt = node->Nmap.find(actions[i]);
#ifdef BOARD_HEURISTIC_ACTIVATED
        std::map<std::string, float>::iterator qsIt = node->Qsquigglemap.find(actions[i]);
        // std::map<std::string, float>::iterator nsIt = node->Nsquigglemap.find(actions[i]);
#endif
        if (nIt == node->Nmap.end())
        {
            // treeLock->lock();
            node->Nmap.emplace(actions[i], 0);
            // treeLock->unlock();
        }
        if (qIt == node->Qmap.end())
        {
            node->Qmap.emplace(actions[i], 0);
        }
#ifdef BOARD_HEURISTIC_ACTIVATED
        if (qsIt == node->Qsquigglemap.end())
        {
            // treeLock->lock();
            node->Qsquigglemap.emplace(actions[i], 0);
            // treeLock->unlock();
        }
        /*if (nsIt == node->Nsquigglemap.end())
        {
            node->Nsquigglemap.emplace(actions[i], 0);
        }*/
#endif
        // treeLock->lock_shared();
        node->Nmap.at(actions[i]) += 1;
        node->Qmap.at(actions[i]) += (z - node->Qmap.at(actions[i])) / node->Nmap.at(actions[i]);
        /*if (node->Qmap.at(actions[i]) != node->Qmap.at(actions[i]))
        {
            Utils::print("Bad things have happened", true);
            Utils::print(std::to_string(z) + " | " + std::to_string(node->Qmap.at(actions[i])) + " | " + std::to_string(node->Nmap.at(actions[i])), true);
        }*/
        // Utils::print(std::to_string(node->Qmap.at(actions[i])) + " | " + std::to_string(z), true);
#ifdef BOARD_HEURISTIC_ACTIVATED
        // node->Nsquigglemap.at(actions[i]) += 1;
        //  node->Qsquigglemap.at(actions[i]) += (m - node->Qsquigglemap.at(actions[i])) / node->Nsquigglemap.at(actions[i]);
        node->Qsquigglemap.at(actions[i]) += ((m /* - rootNode->nodeQsquiggle*/) - node->Qsquigglemap.at(actions[i])) / node->Nmap.at(actions[i]);
#endif
        // treeLock->unlock_shared();
    }
    treeLock->unlock();
    // Utils::print("Ended backup", true);
}

void newNode(BoardState_T boardState, bool whiteToPlay, bool isWhite, std::map<std::string, Node> *tree, std::shared_mutex *treeLock)
{
    std::string posIndex = boardState.dumpPos() + (whiteToPlay ? 'w' : 'b');
    treeLock->lock();
    tree->emplace(posIndex, Node());
#ifdef BOARD_HEURISTIC_ACTIVATED
    // tree->at(posIndex).nodeQsquiggle = stolenHeuristic(boardState, std::vector<BoardState_T::xMove>(), isWhite);
    tree->at(posIndex).nodeQsquiggle = simpleHeuristic(&boardState, isWhite);
#endif
    treeLock->unlock();
}

std::vector<BoardState_T::xMove> defaultPolicy(BoardState_T boardState, bool whiteToPlay, bool simpleMoveGeneration)
{
    std::shared_ptr<std::vector<std::vector<BoardState_T::xMove>>> legalMoves = boardState.getMoves(whiteToPlay, false, simpleMoveGeneration);
    return legalMoves->at(rand() % legalMoves->size());
    std::vector<int> indices = std::vector<int>();
    for (int i = 0; i < legalMoves->size(); i++)
    {
        indices.emplace_back(i);
    }
    std::shuffle(indices.begin(), indices.end(), std::random_device());
    for (int i = 0; i < legalMoves->size(); i++)
    {
        /*!isLoosing(boardState, whiteToPlay, legalMoves->at(indices[i]))*/
        if (legalMoves->at(indices[i]).size() > 0 && !isLoosing(boardState, whiteToPlay, legalMoves->at(indices[i])))
        {
            return legalMoves->at(indices[i]);
        }
    }
    return std::vector<BoardState_T::xMove>();
}

std::vector<BoardState_T::xMove> defaultPolicy_halfSplitGenerator(BoardState_T boardState, bool whiteToPlay)
{
    uint8_t board[13][13];
    boardState.copyBoard(&board);
    std::shared_ptr<std::vector<std::vector<Utils::xMove>>> legalMoves = Utils::getMoves_halfSplit_noPush(whiteToPlay, &board);
    std::vector<int> indices = std::vector<int>();
    for (int i = 0; i < legalMoves->size(); i++)
    {
        indices.emplace_back(i);
    }
    std::shuffle(indices.begin(), indices.end(), std::random_device());
    for (int i = 0; i < legalMoves->size(); i++)
    {
        std::vector<BoardState_T::xMove> move = std::vector<BoardState_T::xMove>();

        for (int j = 0; j < legalMoves->at(indices[i]).size(); j++)
        {
            move.emplace_back(BoardState_T::xMove(legalMoves->at(indices[i]).at(j).i, legalMoves->at(indices[i]).at(j).j, legalMoves->at(indices[i]).at(j).delta));
        }
        if (!isLoosing(boardState, whiteToPlay, move))
        {
            return move;
        }
    }
    return std::vector<BoardState_T::xMove>();
}

std::string stringify(std::vector<BoardState_T::xMove> move)
{
    std::string output = "";
    output.reserve(60);
    for (int i = 0; i < move.size(); i++)
    {
        output += std::to_string(move[i].delta) + "-" + std::to_string(move[i].i) + "-" + std::to_string(move[i].j) + "|";
    }
    return output;
}

bool isLoosing(BoardState_T boardState, bool whiteToPlay, std::vector<BoardState_T::xMove> move)
{
    boardState.unsafeMakeMove(&move);
    BoardState_T::winValue winState = boardState.gameOver(whiteToPlay);
    return winState == (whiteToPlay ? BoardState_T::winValue::black : BoardState_T::winValue::white);
}

float heuristic(BoardState_T boardState, std::vector<BoardState_T::xMove> move)
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

float simpleCaptureHeuristic(std::vector<BoardState_T::xMove> move)
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

float staticHeuristic(BoardState_T *boardState)
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
    return log(abs(whiteScore - blackScore) + 1) * (whiteScore > blackScore ? 1 : -1);
}

float stolenHeuristic(BoardState_T boardState, std::vector<BoardState_T::xMove> move, bool isWhiteTurn)
{
    boardState.unsafeMakeMove(&move);
    uint8_t pieces[13][13];
    boardState.copyBoard(&pieces);
    int eval = Utils::felixEvalWrapper(isWhiteTurn, &pieces);
    return log(abs(eval) + 1) * (eval < 0 ? -1 : 1);
}

float simpleHeuristic(BoardState_T *boardState, bool isWhite)
{
    BoardState_T::winValue winValue = boardState->gameOver(isWhite);
    if (winValue != BoardState_T::winValue::none)
    {
        if (winValue == BoardState_T::winValue::white)
        {
            return isWhite ? 1000000 : -1000000;
        }
        else if (winValue == BoardState_T::winValue::black)
        {
            return isWhite ? -1000000 : 1000000;
        }
        else
        {
            return 0;
        }
    }
    uint8_t pieces[13][13];
    boardState->copyBoard(&pieces);
    float whiteScore = 0;
    float blackScore = 0;
    for (int i = 0; i < 13; i++)
    {
        for (int j = 0; j < 13; j++)
        {
            uint8_t piece = pieces[i][j];
            if (Piece::isTower(piece))
            {
                float towerValues[] = {0, 1, 2.1, 3.2, 4.1, 4.9};
                float towerValue = towerValues[Piece::height(piece)];
                if (Piece::isWhite(piece))
                {
                    whiteScore += towerValue;
                }
                else
                {
                    blackScore += towerValue;
                }
            }
        }
    }
    float unscaledScore = (whiteScore - blackScore);
    return log(abs(unscaledScore) + 1) * (unscaledScore < 0 ? -1 : 1) * (isWhite ? 1 : -1) * 100;
}

#ifdef KILLER_MOVES_ACTIVATED
void spreadKillerMoves(std::vector<std::string> killerMoves, std::map<std::string, Node> *tree)
{
    for (auto it = tree->begin(); it != tree->end(); it++)
    {
        Node *node = &tree->at(it->first);
        for (int i = 0; i < killerMoves.size(); i++)
        {
            if (node->Nmap.contains(killerMoves.at(i)))
            {

                if (!node->Kmap.contains(killerMoves.at(i)))
                {
                    // node->KNmap.emplace(killerMoves.at(i), 0);
                    node->Kmap.emplace(killerMoves.at(i), 0);
                }
                // node->KNmap.at(killerMoves.at(i)) += 1;
                node->Kmap.at(killerMoves.at(i)) += 1;
            }
        }
    }
}
#endif