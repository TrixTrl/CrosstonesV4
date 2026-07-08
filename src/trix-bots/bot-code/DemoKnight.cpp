#include "DemoKnight.h"
#include <thread>
#include <chrono>
#include "globals/Piece.h"

void DemoKnight::getMoveToPlay(uint8_t(*board)[13][13], bool isWhite, int endTime)
{
	if (n < 8) {
		if (isWhite) {
			(*board)[positions[n][0]][positions[n][1]] ^= 0b00000001;
			(*board)[positions[n][2]][positions[n][3]] ^= 0b00000001;
		}
		else {
			Utils::print(12 - positions[n][1], true);
			(*board)[positions[n][0]][12-positions[n][1]] ^= 0b00100001;
			switch (n) {
			case 0:
			case 3:
			case 6:
			case 7:
				(*board)[positions[n][0]][12 - positions[n][1]] ^= 0b00100000;
			}
			(*board)[positions[n][2]][12-positions[n][3]] ^= 0b00100001;
		}
		n++;
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
}
