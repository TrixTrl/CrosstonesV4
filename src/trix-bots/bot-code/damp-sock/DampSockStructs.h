#pragma once

struct DampNode
{
    DampNode(float evaluation, int depth) : evaluation(evaluation), depth(depth), usedInRecentSearch(true) {};
    float evaluation;
    int depth;
    bool usedInRecentSearch;
};