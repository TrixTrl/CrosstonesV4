#pragma once

#include "TrixBoardState.h"
#include "Utils.h"
#include "FastMoveGenerator.h"
#include "MCTS.h"
#include <chrono>

void runPosition(uint8_t (*pieces)[13][13], bool isWhite);
void compareGenerators();
void executionSpeedTest();