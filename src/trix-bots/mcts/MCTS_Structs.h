#pragma once

#include <vector>
#include <map>
#include <string>
#include "Toggles.h"

struct simTreeResult
{
    std::vector<std::string> states = std::vector<std::string>();
    std::vector<std::string> actions = std::vector<std::string>();
#ifdef KILLER_MOVES_ACTIVATED
    std::vector<std::string> killerMoves = std::vector<std::string>();
#endif
    bool whiteToPlay = false;
    float heuristic = 0;
    float z = 0;
};
struct Node
{
    int N = 0;
    std::map<std::string, float> Nmap = std::map<std::string, float>();
    std::map<std::string, float> Qmap = std::map<std::string, float>();
#ifdef BOARD_HEURISTIC_ACTIVATED
    std::map<std::string, float> Qsquigglemap = std::map<std::string, float>();
    // std::map<std::string, float> Nsquigglemap = std::map<std::string, float>();
    float nodeQsquiggle = 0;
#endif
#ifdef KILLER_MOVES_ACTIVATED
    //std::map<std::string, float> KNmap = std::map<std::string, float>();
    std::map<std::string, float> Kmap = std::map<std::string, float>();
#endif
    bool recentlyVisited = true;
};