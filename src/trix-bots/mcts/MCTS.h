#pragma once

#include "Toggles.h"
#include "../utils/TrixBoardState.h"
#include <vector>
#include <map>
#include <time.h>
#include <string>
#include "../utils/Utils.h"
#include <algorithm>
#include "MCTS_Structs.h"
#include <algorithm>
#include <random>
#include "globals/Piece.h"
#include <thread>
#include <shared_mutex>


std::vector<BoardState_T::xMove> UCT_Search(BoardState_T boardState, int searchTime, bool isWhite, std::map<std::string, Node> *tree);
void simulatorLoop(int endTime, BoardState_T boardState, bool isWhite, std::map<std::string, Node> *tree, std::shared_mutex *treeLock);
void simulate(BoardState_T boardState, bool isWhite, std::map<std::string, Node> *tree, std::shared_mutex *treeLock);
simTreeResult simTree(BoardState_T *boardState, bool isWhite, std::map<std::string, Node> *tree, std::shared_mutex *treeLock);
float simDefault(BoardState_T boardState, bool isWhite, bool whiteToPlay);
std::vector<BoardState_T::xMove> selectMove(BoardState_T boardState, bool isWhite, bool enemyMove, float c, std::map<std::string, Node> *tree, simTreeResult *result = nullptr, std::shared_mutex *treeLock = nullptr);
void backup(std::vector<std::string> states, std::vector<std::string> actions, float z, float m, std::map<std::string, Node> *tree, std::shared_mutex *treeLock);
void newNode(BoardState_T boardState, bool whiteToPlay, bool iswhite, std::map<std::string, Node> *tree, std::shared_mutex *treeLock);
std::vector<BoardState_T::xMove> defaultPolicy(BoardState_T boardState, bool whiteToPlay, bool simpleMoveGeneration);
std::vector<BoardState_T::xMove> defaultPolicy_halfSplitGenerator(BoardState_T boardState, bool whiteToPlay);
bool isLoosing(BoardState_T boardState, bool whiteToPlay, std::vector<BoardState_T::xMove> move);
std::string stringify(std::vector<BoardState_T::xMove>);
#ifdef KILLER_MOVES_ACTIVATED
void spreadKillerMoves(std::vector<std::string> killerMoves, std::map<std::string, Node> *tree);
#endif

float heuristic(BoardState_T boardState, std::vector<BoardState_T::xMove> move);
float simpleCaptureHeuristic(std::vector<BoardState_T::xMove> move);
float staticHeuristic(BoardState_T *boardState);
float stolenHeuristic(BoardState_T boardState, std::vector<BoardState_T::xMove> move, bool isWhiteTurn);
float simpleHeuristic(BoardState_T *boardState, bool isWhite);