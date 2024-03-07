#include "Hydra.h"
#include "../Globals/Player.h"
#include "Utils.h"
#include <thread>
#include <chrono>
#include <vector>
#include <memory>
#include "../Test Bots/BasicGenerator.h"

void Hydra::getMoveToPlay(uint8_t(*board)[13][13], bool isWhite, int endTime)
{
	int bestMove = Utils::getBestMoveThreaded(isWhite, board, depth, threadCount);
	std::shared_ptr<std::vector<std::vector<BasicGenerator::xMove>>> moves;
	moves = BasicGenerator::getMoves(isWhite, board);
	for (int i = 0; i < ((*moves)[bestMove]).size(); i++) {
		(*board)[((*moves)[bestMove])[i].i][((*moves)[bestMove])[i].j] ^= ((*moves)[bestMove])[i].delta;
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
}
