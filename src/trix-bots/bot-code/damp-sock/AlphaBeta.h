#pragma once

#include "../../utils/Utils.h"
#include "Evaluation.h"
#include "../../utils/TrixBoardState.h"
#include "../../utils/FastMoveGenerator.h"
#include <vector>
#include "DampSockStructs.h"

/*
function alphabeta(node, depth, α, β, maximizingPlayer) is
    if depth == 0 or node is terminal then
        return the heuristic value of node
    if maximizingPlayer then
        value := −∞
        for each child of node do
            value := max(value, alphabeta(child, depth − 1, α, β, FALSE))
            if value ≥ β then
                break (* β cutoff *)
            α := max(α, value)
        return value
    else
        value := +∞
        for each child of node do
            value := min(value, alphabeta(child, depth − 1, α, β, TRUE))
            if value ≤ α then
                break (* α cutoff *)
            β := min(β, value)
        return value
*/

std::pair<bool, float> alphaBeta_sockVersion(uint8_t (*pieces)[13][13], int depth, float alpha, float beta, bool maximizing, std::map<std::string, DampNode> &table, int endOfThinkingTime);