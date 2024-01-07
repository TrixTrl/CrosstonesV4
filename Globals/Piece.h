#pragma once
#ifndef PIECE_H
#define PIECE_H

#include <cstdint>


class Piece {
private:
	static const uint8_t heightMask = 0b00000111;
	static const uint8_t addOnMask =  0b00011000;
	static const uint8_t colourMask = 0b00100000;

public:
	//Addon colors
	static const uint8_t Blue = 8;
	static const uint8_t Red = 16;

	//Piece colors
	static const uint8_t Black = 32;
	static const uint8_t White = 0;

	static const uint8_t turnPiece = 128;
	static const uint8_t setTurnPiece = 64;

	//Piece Types
	static const uint8_t None = 0;
	static const uint8_t normal1 = 1;
	static const uint8_t normal2 = 2;
	static const uint8_t normal3 = 3;
	static const uint8_t normal4 = 4;
	static const uint8_t normal5 = 5;
	static const uint8_t blue1 = 1 | Blue;
	static const uint8_t blue2 = 2 | Blue;
	static const uint8_t blue3 = 3 | Blue;
	static const uint8_t blue4 = 4 | Blue;
	static const uint8_t blue5 = 5 | Blue;
	static const uint8_t red1 = 1 | Red;
	static const uint8_t red2 = 2 | Red;
	static const uint8_t red3 = 3 | Red;
	static const uint8_t red4 = 4 | Red;
	static const uint8_t red5 = 5 | Red;

	//Pieces
	static const uint8_t whiteNormal1 = normal1 | White;
	static const uint8_t whiteNormal2 = normal2 | White;
	static const uint8_t whiteNormal3 = normal3 | White;
	static const uint8_t whiteNormal4 = normal4 | White;
	static const uint8_t whiteNormal5 = normal5 | White;
	static const uint8_t whiteBlue2 = blue2 | White;
	static const uint8_t whiteBlue3 = blue3 | White;
	static const uint8_t whiteBlue4 = blue4 | White;
	static const uint8_t whiteBlue5 = blue5 | White;
	static const uint8_t whiteRed2 = red2 | White;
	static const uint8_t whiteRed3 = red3 | White;
	static const uint8_t whiteRed4 = red4 | White;
	static const uint8_t whiteRed5 = red5 | White;

	static const uint8_t blackNormal1 = normal1 | Black;
	static const uint8_t blackNormal2 = normal2 | Black;
	static const uint8_t blackNormal3 = normal3 | Black;
	static const uint8_t blackNormal4 = normal4 | Black;
	static const uint8_t blackNormal5 = normal5 | Black;
	static const uint8_t blackBlue2 = blue2 | Black;
	static const uint8_t blackBlue3 = blue3 | Black;
	static const uint8_t blackBlue4 = blue4 | Black;
	static const uint8_t blackBlue5 = blue5 | Black;
	static const uint8_t blackRed2 = red2 | Black;
	static const uint8_t blackRed3 = red3 | Black;
	static const uint8_t blackRed4 = red4 | Black;
	static const uint8_t blackRed5 = red5 | Black;

	static const uint8_t pieceIndices[27];
	//full indices from 0 to 169
	static const uint8_t turnPiecePositions[20];
	static const uint8_t turnPieceIndices[169];

	static const uint8_t colour(uint8_t const piece) { return (piece & colourMask); }
	static const uint8_t height(uint8_t const piece) { return (piece & heightMask); }
	static const uint8_t addOn(uint8_t const piece) { return (piece & addOnMask); }
	static const uint8_t tower(uint8_t const piece) { return (piece & (colourMask | heightMask | addOnMask)); }

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