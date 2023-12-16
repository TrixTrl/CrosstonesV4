#include <iostream>
#include "../BoardState.h"
#include "../BoardState.cpp"
#include "../Piece.h"
#include "../Piece.cpp"
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