#include "Piece.h"

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