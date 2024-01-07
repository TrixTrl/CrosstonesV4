#include "Piece.h"

const uint8_t Piece::pieceIndices[] =
{
	None,
	whiteNormal1, whiteNormal2, whiteNormal3, whiteNormal4, whiteNormal5, 
	whiteBlue2, whiteBlue3, whiteBlue4, whiteBlue5, whiteRed2, whiteRed3, whiteRed4, whiteRed5,
	blackNormal1, blackNormal2, blackNormal3, blackNormal4, blackNormal5, 
	blackBlue2, blackBlue3, blackBlue4, blackBlue5, blackRed2, blackRed3, blackRed4, blackRed5
};
const uint8_t Piece::turnPiecePositions[] =
{
		   2,		6,		 10,
	 26,  28,	   32,       36,  38,
		  80, 82,		86,  88,
	130, 132,	  136,		140, 142,
		 158,	  162,		166
};
const uint8_t Piece::turnPieceIndices[] =
{
	0,0,1,0,0,0,2,0,0,0,3,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,
	4,0,5,0,0,0,6,0,0,0,7,0,8,
	0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,9,0,10,0,0,0,11,0,12,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,
	13,0,14,0,0,0,15,0,0,0,16,0,17,
	0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,18,0,0,0,19,0,0,0,20,0,0
};

uint8_t Piece::maxSteps(uint8_t piece) {
	uint8_t maxSteps = 0;
	switch (height(piece)) {
	case 1:
		maxSteps = 4;
		break;
	case 2:
	case 3:
		maxSteps = 3;
		break;
	case 4:
	case 5:
		maxSteps = 2;
		break;
	default:
		maxSteps = 0;
		break;
	}
	if (isRed(piece)) maxSteps++;

	return maxSteps;
}

bool Piece::isColour(uint8_t piece, int colour) {
	return (piece & colourMask) == colour;
}

bool Piece::isWhite(uint8_t piece) {
	return (piece & colourMask) == White;
}

bool Piece::isWhiteTower(uint8_t piece) {
	return ((piece & colourMask) == White) && (height(piece) != 0);
}

bool Piece::hasAddOn(uint8_t piece) {
	return (piece & addOnMask);
}
bool Piece::isBlue(uint8_t piece) {
	return piece & Piece::Blue;
}
bool Piece::isRed(uint8_t piece) {
	return piece & Piece::Red;
}

bool Piece::isHeight(uint8_t piece, uint8_t h) {
	return (piece & heightMask) == h;
}

bool Piece::isTower(uint8_t piece) {
	return !isHeight(piece, 0) && (addOn(piece) == 0 || !isHeight(piece, 1));
}

bool Piece::isAddOn(uint8_t piece) {
	return (addOn(piece) != 0) && isHeight(piece, 1);
}