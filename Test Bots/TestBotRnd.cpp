#include "TestBotRnd.h"
#include "BasicGenerator.h"
#include "../Globals/Player.h"
#include <thread>
#include <chrono>

void TestBotRnd::getMoveToPlay(uint8_t(*board)[13][13], bool isWhite, int endTime)
{
	std::shared_ptr<std::vector<std::vector<BasicGenerator::xMove>>> moves;
	moves = BasicGenerator::getMoves(isWhite, board);
	int random = (rand() % (moves->size()-1))+1;
	for (int i = 0; i < ((*moves)[random]).size(); i++) {
		(*board)[((*moves)[random])[i].i][((*moves)[random])[i].j] ^= ((*moves)[random])[i].delta;
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
}
