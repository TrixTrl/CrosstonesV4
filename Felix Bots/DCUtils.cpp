#include "DCUtils.h"
#include "../Globals/Piece.h"
#include <vector>

using namespace std;

void DCUtils::applyXMove(uint8_t(*board)[13][13], vector<BasicGenerator::xMove>& move)
{
	for (int i = 0; i < move.size(); i++) {
		(*board)[move[i].i][move[i].j] ^= move[i].delta;
	}
}

inline void DCUtils::wrapWithXMove(uint8_t(*board)[13][13], vector<BasicGenerator::xMove>& move, 
	function<void()> action) 
{
	applyXMove(board, move);
	action();
	applyXMove(board, move);
}

DCUtils::WinValue DCUtils::calcWinValue(uint8_t(*board)[13][13], bool isWhite) 
{
	// Totally not copied from trix
	bool bases[2][2] = { {false, false}, {false, false} }; //[0] white   [1] black   [x][0] white base   [x][1] black base
	for (int i = 5; i < 8; i++) {
		for (int j = 0; j < 4; j++) {
			if (Piece::isTower((*board)[i][j])) {
				if (Piece::isWhite((*board)[i][j])) {
					bases[0][1] = true;	//black base white piece
				}
				else {
					bases[1][1] = true;	//black base black piece
				}
			}

			if (Piece::isTower((*board)[i][12 - j])) {
				if (Piece::isWhite((*board)[i][12 - j])) {
					bases[0][0] = true;	//white base white piece
				}
				else {
					bases[1][0] = true;	//white base black piece
				}
			}
		}
	}

	bool whiteWin = (bases[0][1] && !bases[1][1]);
	bool blackWin = (bases[1][0] && !bases[0][0]);

	if (whiteWin) {
		return (blackWin ? WinValue::draw : WinValue::white);
	}
	return (blackWin ? WinValue::black : WinValue::none);
}

bool DCUtils::gameOver(uint8_t(*board)[13][13]) 
{
	// check for whin in black base
	bool blackBaseContested = false;
	for (int i = 5; i < 8; i++) {
		for (int j = 0; j < 4; j++) {
			if (Piece::isTower((*board)[i][j])) {
				if (Piece::isWhite((*board)[i][j])) {
					blackBaseContested = true;
				}
				else {
					goto blackBaseSafe;
				}
			}
		}
	}
	if (blackBaseContested) 
		return true;
blackBaseSafe:

	// check for whin in white base
	bool whiteBaseContested = false;
	for (int i = 5; i < 8; i++) {
		for (int j = 0; j < 4; j++) {
			if (Piece::isTower((*board)[i][12 - j])) {
				if (Piece::isWhite((*board)[i][12 - j])) {
					goto whiteBaseSafe;
				}
				else {
					whiteBaseContested = true;
				}
			}
		}
	}
	if (whiteBaseContested)
		return true;
whiteBaseSafe:
	return false;
}