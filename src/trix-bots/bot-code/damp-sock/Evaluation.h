#pragma once

#include "../../utils/Utils.h"
#include "../../../globals/Piece.h"

class Evaluator
{
public:
    // The evaluation of the position from White's perspecctive
    static float basicEvaluation(const uint8_t (*pieces)[13][13]);
};