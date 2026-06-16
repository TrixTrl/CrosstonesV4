#pragma once

#include "../BoardState.h"
#include <vector>
#include <map>
#include <time.h>

using namespace ::std;

struct simTreeResult
{
    vector<string> states = vector<string>();
    bool whiteToPlay = false;
};
struct Node
{
    int N = 0;
    map<string, float> Nmap = map<string, float>();
    map<string, float> Qmap = map<string, float>();
};

vector<BoardState::xMove> UCT_Search(BoardState boardState, int searchTime, bool isWhite, map<string, Node> *tree);
void simulate(BoardState boardState, bool isWhite, map<string, Node> *tree);
simTreeResult simTree(BoardState boardState, bool isWhite, map<string, Node> *tree);
float simDefault(BoardState boardState, bool isWhite, bool whiteToPlay);
vector<BoardState::xMove> selectMove(BoardState boardState, bool isWhite, bool enemyMove, float c, map<string, Node> *tree);
void backup(vector<string> states, float z, map<string, Node> *tree);
void newNode(BoardState boardState, bool whiteToPlay, map<string, Node> *tree);
vector<BoardState::xMove> defaultPolicy(BoardState boardState, bool whiteToPlay);
string stringify(vector<BoardState::xMove>);