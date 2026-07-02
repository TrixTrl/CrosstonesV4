#include "StateTester.h"
#include "globals/Piece.h"
#include <iostream>

#define PIECEREDUCTIONERROR 0

StateTester::StateTester(Board bs)
{
	bs.copyBoard(&pieces);
	BS = bs;
}

bool StateTester::staticLegality()
{
	bool passing = true;
	for (int i = 0; i < 13; i++) {
		for (int j = 0; j < 13; j++) {
			uint8_t piece = pieces[i][j];

			if (Piece::height(piece) > 5) {
				error += "static legality failure : piece height violation " + std::to_string(Piece::height(piece)) + " at " + std::to_string(i) + " | " + std::to_string(j) + "\n";
				passing = false;
			}
			if (Piece::isBlue(piece) && Piece::isRed(piece)) {
				error += "static legality failure : simultaneous addon violation at " + std::to_string(i) + " | " + std::to_string(j) + "\n";
				passing = false;
			}
			if (Piece::height(piece) == 0 && Piece::isColour(piece, Piece::Black)) {
				error += "static legality failure : remaining color violation at " + std::to_string(i) + " | " + std::to_string(j) + "\n";
				passing = false;
			}
		}
	}
	return passing;
}

int StateTester::pieceCount()
{
	int n = 0;
	for (int i = 0; i < 13; i++) {
		for (int j = 0; j < 13; j++) {
			uint8_t piece = pieces[i][j];
			n += Piece::height(piece);
		}
	}
	return n;
}

int StateTester::redCount()
{
	int n = 0;
	for (int i = 0; i < 13; i++) {
		for (int j = 0; j < 13; j++) {
			uint8_t piece = pieces[i][j];
			n += (Piece::isRed(piece) != 0);
		}
	}
	return n;
}

int StateTester::blueCount()
{
	int n = 0;
	for (int i = 0; i < 13; i++) {
		for (int j = 0; j < 13; j++) {
			uint8_t piece = pieces[i][j];
			n += (Piece::isBlue(piece) != 0);
		}
	}
	return n;
}

bool StateTester::checkAllMoves()
{
	bool passing = true;
	std::shared_ptr<std::vector<std::vector<Board::xMove>>> moves;
	moves = BS.getMoves(true);
	int baseCount = pieceCount();
	int baseBlueCount = blueCount();
	int baseRedCount = redCount();
	for (int i = 0; i < moves->size(); i++) {
		BS.unsafeMakeMove(&(*moves)[i]);
		BS.copyBoard(&pieces);
		BS.unsafeMakeMove(&(*moves)[i]);
		bool staticCheck = staticLegality();
		if (!staticCheck) {
			error += "		error on white move " + std::to_string(i) + "\n";
		}
		passing &= staticCheck;
		int newCount = pieceCount();
		int newBlueCount = blueCount();
		int newRedCount = redCount();
		if (newCount > baseCount) {
			passing = false;
			error += "piece number error : increase in pieces from " + std::to_string(baseCount) + " to " + std::to_string(newCount) + " on white move " + std::to_string(i) + "\n";
		}
		if (newCount < baseCount && PIECEREDUCTIONERROR) {
			passing = false;
			error += "piece number error : decrease in pieces from " + std::to_string(baseCount) + " to " + std::to_string(newCount) + " on white move " + std::to_string(i) + "\n";
		}
		if (newBlueCount > baseBlueCount) {
			passing = false;
			error += "addon number error : increase in blue addons from " + std::to_string(baseBlueCount) + " to " + std::to_string(newBlueCount) + " on white move " + std::to_string(i) + "\n";
		}
		if (newBlueCount < baseBlueCount && PIECEREDUCTIONERROR) {
			passing = false;
			error += "addon number error : decrease in blue addons from " + std::to_string(baseBlueCount) + " to " + std::to_string(newBlueCount) + " on white move " + std::to_string(i) + "\n";
		}
		if (newRedCount > baseRedCount) {
			passing = false;
			error += "addon number error : increase in red addons from " + std::to_string(baseRedCount) + " to " + std::to_string(newRedCount) + " on white move " + std::to_string(i) + "\n";
		}
		if (newRedCount < baseRedCount && PIECEREDUCTIONERROR) {
			passing = false;
			error += "addon number error : decrease in red addons from " + std::to_string(baseRedCount) + " to " + std::to_string(newRedCount) + " on white move " + std::to_string(i) + "\n";
		}
	}
	moves = BS.getMoves(false);
	for (int i = 0; i < moves->size(); i++) {
		BS.unsafeMakeMove(&(*moves)[i]);
		BS.copyBoard(&pieces);
		BS.unsafeMakeMove(&(*moves)[i]);
		bool staticCheck = staticLegality();
		if (!staticCheck) {
			error += "		error on black move " + std::to_string(i) + "\n";
		}
		passing &= staticCheck;
		int newCount = pieceCount();
		int newBlueCount = blueCount();
		int newRedCount = redCount();
		if (newCount > baseCount) {
			passing = false;
			error += "piece number error : increase in pieces from " + std::to_string(baseCount) + " to " + std::to_string(newCount) + " on black move " + std::to_string(i) + "\n";
		}
		if (newCount < baseCount && PIECEREDUCTIONERROR) {
			passing = false;
			error += "piece number error : decrease in pieces from " + std::to_string(baseCount) + " to " + std::to_string(newCount) + " on black move " + std::to_string(i) + "\n";
		}
		if (newBlueCount > baseBlueCount) {
			passing = false;
			error += "addon number error : increase in blue addons from " + std::to_string(baseBlueCount) + " to " + std::to_string(newBlueCount) + " on black move " + std::to_string(i) + "\n";
		}
		if (newBlueCount < baseBlueCount && PIECEREDUCTIONERROR) {
			passing = false;
			error += "addon number error : decrease in blue addons from " + std::to_string(baseBlueCount) + " to " + std::to_string(newBlueCount) + " on black move " + std::to_string(i) + "\n";
		}
		if (newRedCount > baseRedCount) {
			passing = false;
			error += "addon number error : increase in red addons from " + std::to_string(baseRedCount) + " to " + std::to_string(newRedCount) + " on black move " + std::to_string(i) + "\n";
		}
		if (newRedCount < baseRedCount && PIECEREDUCTIONERROR) {
			passing = false;
			error += "addon number error : decrease in red addons from " + std::to_string(baseRedCount) + " to " + std::to_string(newRedCount) + " on black move " + std::to_string(i) + "\n";
		}
	}
	return passing;
}

bool StateTester::fullTest() {
	bool passing = true;
	passing &= staticLegality();
	passing &= checkAllMoves();


	if (error.size() == 0) {
		error = "No errors detected";
	}

	std::cout << error;
	return passing;
}