#pragma once

#include "../BoardState.h"
#include <vector>
#include <map>
#include <time.h>
#include <string>
#include "Utils.h"
#include <algorithm>
#include "MCTS_Structs.h"

// using namespace ::std;

std::vector<BoardState::xMove> UCT_Search(BoardState boardState, int searchTime, bool isWhite, std::map<std::string, Node> *tree);
void simulate(BoardState boardState, bool isWhite, std::map<std::string, Node> *tree);
simTreeResult simTree(BoardState *boardState, bool isWhite, std::map<std::string, Node> *tree);
float simDefault(BoardState boardState, bool isWhite, bool whiteToPlay);
std::vector<BoardState::xMove> selectMove(BoardState boardState, bool isWhite, bool enemyMove, float c, std::map<std::string, Node> *tree, simTreeResult *result = nullptr);
void backup(std::vector<std::string> states, std::vector<std::string> actions, float z, std::map<std::string, Node> *tree);
void newNode(BoardState boardState, bool whiteToPlay, std::map<std::string, Node> *tree);
std::vector<BoardState::xMove> defaultPolicy(BoardState boardState, bool whiteToPlay);
std::string stringify(std::vector<BoardState::xMove>);