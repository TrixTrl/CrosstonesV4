#pragma once
#include <Windows.h>
#include <stdint.h>

class UI

{
private:
	bool cellHighlights[2][13][13];
public:
	const int cellSize = 55;
	const int boardSize = cellSize * 13;
	const POINT boardPosition = { 20, 20 };


	
	POINT latestClickedPos = { -1, -1 };
	POINT latestClickedGridPos = { -1, -1 };

	void setHighlight(int x, int y);
	void clearHighlight(int x, int y);
	void clearAllHighlights();

	void paint(HWND hwnd, bool drawBackground, uint8_t board[2][13][13]);

	POINT lclick(HWND hwnd, POINT pos);



};

