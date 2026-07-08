#include "StateTester.h"
#include "globals/Piece.h"
#include <iostream>
#include <bitset>

#define PIECEREDUCTIONERROR 0

StateTester::StateTester() {
    std::bitset<3> set(5);
    BS.rst(set);
    BS.copyPositionTo(&pieces);
}

bool StateTester::staticLegality()
{
    bool passing = true;
    for (int i = 0; i < 13; i++) {
        for (int j = 0; j < 13; j++) {
            uint8_t piece = pieces[i][j];
            if (Piece::height(piece) > 5) {
				error_ += "Found piece with height > 5 at (" + std::to_string(i) + ", " + std::to_string(j) + ")\n";
                passing = false;
            } 
			if (Piece::isBlue(piece) && Piece::isRed(piece)) {
				error_ += "Found piece with both blue and red addon at (" + std::to_string(i) + ", " + std::to_string(j) + ")\n";
				passing = false;
			} 
			if (Piece::height(piece) == 0 && Piece::isColour(piece, Piece::Black)) {
				error_ += "Found colored empty tile at (" + std::to_string(i) + ", " + std::to_string(j) + ")\n";
				passing = false;
			}
        }
    }
    return passing;
}

int StateTester::pieceCount()
{
    int count = 0;
    for (int i = 0; i < 13; i++) {
        for (int j = 0; j < 13; j++) {
            uint8_t piece = pieces[i][j];
            count += Piece::height(piece);
        }
    }
    return count;
}

int StateTester::redCount()
{
	int count = 0;
	for (int i = 0; i < 13; i++) {
		for (int j = 0; j < 13; j++) {
			uint8_t piece = pieces[i][j];
			if (Piece::isRed(piece))
				++count;
		}
	}
	return count;
}

int StateTester::blueCount()
{
	int count = 0;
	for (int i = 0; i < 13; i++) {
		for (int j = 0; j < 13; j++) {
			uint8_t piece = pieces[i][j];
			if (Piece::isBlue(piece))
				++count;
		}
	}
	return count;
}

bool StateTester::checkAllMoves()
{
	bool passing = true;
	auto moves = BS.getMoves(true);
	int baseCount = pieceCount();
	int baseBlueCount = blueCount();
	int baseRedCount = redCount();
	for (int i = 0; i < moves.size(); i++) {
		BS.unsafeMakeMove(moves[i]);
		BS.copyPositionTo(&pieces);
		BS.unsafeMakeMove(moves[i]);
		bool staticCheck = staticLegality();
		if (!staticCheck)
		{
			error_ += "Error found in test for move " + std::to_string(i) + "\n";
			passing &= false;
		}
		int newCount = pieceCount();
		int newBlueCount = blueCount();
		int newRedCount = redCount();
		if (newCount > baseCount + PIECEREDUCTIONERROR) {
			error_ += "Error in move " + std::to_string(i) + ": piece count increased (" + std::to_string(newCount) + " > " + std::to_string(baseCount) + ")\n";
			passing = false;
		}
		if (newBlueCount > baseBlueCount + PIECEREDUCTIONERROR) {
			error_ += "Error in move " + std::to_string(i) + ": blue count increased (" + std::to_string(newBlueCount) + " > " + std::to_string(baseBlueCount) + ")\n";
			passing = false;
		}
		if (newRedCount > baseRedCount + PIECEREDUCTIONERROR) {
			error_ += "Error in move " + std::to_string(i) + ": red count increased (" + std::to_string(newRedCount) + " > " + std::to_string(baseRedCount) + ")\n";
			passing = false;
		}
	}
	moves = BS.getMoves(false);
	baseCount = pieceCount();
	baseBlueCount = blueCount();
	baseRedCount = redCount();
	for (int i = 0; i < moves.size(); i++) {
		BS.unsafeMakeMove(moves[i]);
		BS.copyPositionTo(&pieces);
		BS.unsafeMakeMove(moves[i]);
		bool staticCheck = staticLegality();
		if (!staticCheck)
		{
			error_ += "Error found in test for move " + std::to_string(i) + "\n";
			passing &= false;
		}
		int newCount = pieceCount();
		int newBlueCount = blueCount();
		int newRedCount = redCount();
		if (newCount > baseCount + PIECEREDUCTIONERROR) {
			error_ += "Error in move " + std::to_string(i) + ": piece count increased (" + std::to_string(newCount) + " > " + std::to_string(baseCount) + ")\n";
			passing = false;
		}
		if (newBlueCount > baseBlueCount + PIECEREDUCTIONERROR) {
			error_ += "Error in move " + std::to_string(i) + ": blue count increased (" + std::to_string(newBlueCount) + " > " + std::to_string(baseBlueCount) + ")\n";
			passing = false;
		}
		if (newRedCount > baseRedCount + PIECEREDUCTIONERROR) {
			error_ += "Error in move " + std::to_string(i) + ": red count increased (" + std::to_string(newRedCount) + " > " + std::to_string(baseRedCount) + ")\n";
			passing = false;
		}
	}
	return passing;
}

bool StateTester::run()
{
    bool passing = true;
    passing &= staticLegality();
    passing &= checkAllMoves();
    if (error_.size() == 0) {
        error_ = "No errors detected";
    }
    std::cout << error_ << "\n";
    return passing;
}

REGISTER_TEST(StateTester)
