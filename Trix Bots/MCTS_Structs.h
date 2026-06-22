#pragma once

#include <vector>
#include <map>
#include <string>

struct simTreeResult
{
    std::vector<std::string> states = std::vector<std::string>();
    std::vector<std::string> actions = std::vector<std::string>();
    bool whiteToPlay = false;
};
struct Node
{
    int N = 0;
    std::map<std::string, float> Nmap = std::map<std::string, float>();
    std::map<std::string, float> Qmap = std::map<std::string, float>();
};