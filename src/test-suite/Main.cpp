#include <iostream>
#include "game-suite/Board.h"
#include "game-suite/Board.cpp"
#include "globals/Piece.h"
#include "globals/Piece.cpp"
#include <bitset>
#include "StateTester.h"


int main() {
	Board bs;
	std::bitset<3> set(5);
	bs.rst(set);

	StateTester tester(bs);

	tester.fullTest();

	return 0;
}