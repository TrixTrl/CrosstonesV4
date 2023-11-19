#include "BoardState.h"
#include <bitset>

int main() {

	BoardState bs;
	std::bitset<3> set(5);
	bs.rst(set);
	bs.print();

	return 0;
}