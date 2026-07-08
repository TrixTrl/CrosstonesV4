#pragma once

#include "TrixBoardState.h"
#include "Utils.h"
#include "FastMoveGenerator.h"
#include "../mcts/MCTS.h"
#include <chrono>
#include <string>

void runPosition(uint8_t (*pieces)[13][13], bool isWhite);
void compareGenerators();
void executionSpeedTest();
void tryPosition(std::string position, int positonIndex);