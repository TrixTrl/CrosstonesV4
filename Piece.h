#pragma once
#ifndef PIECE_H
#define PIECE_H

#include <cstdint>


class Piece {
private:
	static const uint8_t heightMask = 0b00000111;
	static const uint8_t colourMask = 0b00100000;
	static const uint8_t addOnMask = 0b00011000;

public:
	static const int None = 0;

	static const int Blue = 8;
	static const int Red = 16;

	static const int Black = 32;
	static const int White = 0;


	static uint8_t colour(uint8_t piece) { return (piece & colourMask); }
	static uint8_t height(uint8_t piece) { return (piece & heightMask); }
	static uint8_t addOn(uint8_t piece) { return (piece & addOnMask); }

	static bool isColour(uint8_t piece, int colour);
	static bool isWhite(uint8_t piece);
	static bool isWhiteTower(uint8_t piece);
	static bool hasAddOn(uint8_t piece);
	static bool isBlue(uint8_t piece);
	static bool isRed(uint8_t piece);
	static bool isHeight(uint8_t piece, uint8_t h);

	static bool isTower(uint8_t piece);

	static bool isAddOn(uint8_t piece);

	static uint8_t maxSteps(uint8_t piece);
};


#endif