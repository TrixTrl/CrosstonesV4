#include <iostream>
#include "../CrosstonesV4/BoardState.h"
#include "../CrosstonesV4/BoardState.cpp"
#include "../CrosstonesV4/Piece.h"
#include "../CrosstonesV4/Piece.cpp"
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