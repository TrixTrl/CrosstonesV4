#pragma once

#include "TrixBoardState.h"
#include "Utils.h"
#include "FastMoveGenerator.h"

void runPosition(uint8_t (*pieces)[13][13], bool isWhite);
void compareGenerators();