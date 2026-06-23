#pragma once

#include "../BoardState.h"
#include <vector>
#include <map>
#include <time.h>
#include <string>
#include "Utils.h"
#include <algorithm>
#include "MCTS_Structs.h"
#include <algorithm>
#include <random>
#include "../Globals/Piece.h"
#include <thread>
#include <mutex>

// using namespace ::std;

std::vector<BoardState::xMove> UCT_Search(BoardState boardState, int searchTime, bool isWhite, std::map<std::string, Node> *tree);
void simulatorLoop(int endTime, BoardState boardState, bool isWhite, std::map<std::string, Node> *tree, std::mutex *treeLock);
void simulate(BoardState boardState, bool isWhite, std::map<std::string, Node> *tree, std::mutex *treeLock);
simTreeResult simTree(BoardState *boardState, bool isWhite, std::map<std::string, Node> *tree, std::mutex *treeLock);
float simDefault(BoardState boardState, bool isWhite, bool whiteToPlay);
std::vector<BoardState::xMove> selectMove(BoardState boardState, bool isWhite, bool enemyMove, float c, float b, std::map<std::string, Node> *tree, simTreeResult *result = nullptr);
void backup(std::vector<std::string> states, std::vector<std::string> actions, float z, float m, std::map<std::string, Node> *tree);
void newNode(BoardState boardState, bool whiteToPlay, std::map<std::string, Node> *tree);
std::vector<BoardState::xMove> defaultPolicy(BoardState boardState, bool whiteToPlay);
bool isLoosing(BoardState boardState, bool whiteToPlay, std::vector<BoardState::xMove> move);
std::string stringify(std::vector<BoardState::xMove>);
float heuristic(BoardState boardState, std::vector<BoardState::xMove> move);
float simpleCaptureHeuristic(std::vector<BoardState::xMove> move);
float staticHeuristic(BoardState* boardState);
