#include <iostream>
#include "game-suite/BoardState.h"
#include "game-suite/BoardState.cpp"
#include "globals/Piece.h"
#include "globals/Piece.cpp"
#include <bitset>
#include "StateTester.h"


int main() {
	BoardState bs;
	std::bitset<3> set(5);
	bs.rst(set);

	StateTester tester(bs);

	tester.fullTest();

	return 0;
}