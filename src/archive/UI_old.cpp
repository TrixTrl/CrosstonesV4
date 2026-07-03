#include "UI.h"

#include "fix_win32_compatibility.h"
#include <string>
#include "Board.h"
#include "globals/Piece.h"


POINT UI::lclick(HWND hwnd, POINT pos)
{
	//handle board bounds
	if (pos.x < boardPosition.x || pos.x > boardPosition.x + boardSize
		|| pos.y < boardPosition.y || pos.y > boardPosition.y + boardSize)
	{
		latestClickedGridPos = { -1, -1 };
	}
	else
	{
		latestClickedGridPos = {
		(pos.x - boardPosition.x) / cellSize,
		(pos.y - boardPosition.y) / cellSize
		};
		// cellHighlights[latestClickedGridPos.x][latestClickedGridPos.y] = true;
	}
	

	InvalidateRect(hwnd, nullptr, TRUE); // Trigger repaint

	return latestClickedGridPos;
}

void UI::paint(HWND hwnd, bool drawBackground, uint8_t board[2][13][13])
{
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hwnd, &ps);

	// All painting occurs here, between BeginPaint and EndPaint.
	
	// Background
	if (drawBackground)
		FillRect(hdc, &ps.rcPaint, CreateSolidBrush(RGB(0, 0, 255)));

	std::string drawTypes[] = {
	"ahthbht",
	"v v b v",
	"ththbht",
	"v v b v",
	"bbbbbbb",
	"b v v b",
	"ahththa"
	};

	RECT rect;

	enum DrawPath
	{
		NONE = 0, HORIZONTAL, VERTICAL
	};

	//SetRect(&rect, 0, 650, 100, 750);
	//FillRect(hdc, &rect, (HBRUSH)(CreateSolidBrush(RGB(255, 0, 0))));
	//DrawText(hdc, L"Hello World!", -1, &ps.rcPaint, DT_CENTER);

	for (int i = 0; i < 13; i++) {
		for (int j = 0; j < 13; j++) {
			if (!drawBackground
				&& board[0][i][j] == board[1][i][j]
				&& cellHighlights[0][i][j] == cellHighlights[1][i][j])
			{
				continue;
			}

			SetRect(&rect,
				boardPosition.x + i * cellSize,
				boardPosition.y + j * cellSize,
				boardPosition.x + (i + 1) * cellSize,
				boardPosition.y + (j + 1) * cellSize
			);

			HBRUSH col;
			DrawPath path = NONE;
			uint8_t piece = board[0][i][j];		//------------------------------------------------------------------
			switch (drawTypes[j < 7 ? j : 5 - (j % 7)][i < 7 ? i : 5 - (i % 7)]) {
			case 'a':
				col = CreateSolidBrush(RGB(63, 45, 35));
				break;
			case ' ':
				col = CreateSolidBrush(RGB(122, 70, 58));
				break;
			case 'h':
				col = CreateSolidBrush(RGB(214, 176, 116));
				path = DrawPath::HORIZONTAL;
				break;
			case 'v':
				col = CreateSolidBrush(RGB(214, 176, 116));
				path = DrawPath::VERTICAL;
				break;
			case 'b':
				col = CreateSolidBrush(RGB(231, 176, 88));
				break;
			case 't':
				col = CreateSolidBrush(RGB(214, 176, 116));
				path = (piece & Board::setTurnPiece) == 0 ? DrawPath::HORIZONTAL : DrawPath::VERTICAL;
				break;
			default:
				col = CreateSolidBrush(RGB(255, 0, 0));
				break;
			}

			FillRect(hdc, &rect, col);
			switch (path) {
			case DrawPath::NONE:
				break;
			case DrawPath::HORIZONTAL:
				rect.top += cellSize / 3;
				rect.bottom -= cellSize / 3;
				col = CreateSolidBrush(RGB(63, 45, 35));
				FillRect(hdc, &rect, col);
				break;
			case DrawPath::VERTICAL:
				rect.left += cellSize / 3;
				rect.right -= cellSize / 3;
				col = CreateSolidBrush(RGB(63, 45, 35));
				FillRect(hdc, &rect, col);
				break;
			}

			if (Piece::tower(piece)) {
				float piecePadding = 0.1;
				float pieceBorder = 0.07;
				COLORREF colors[4][2] = { {RGB(15, 15, 15), RGB(250, 250, 250)}, {RGB(250, 250, 250), RGB(15, 15, 15)}, {RGB(224, 36, 36), RGB(230, 37, 37)}, {RGB(31, 20, 250), RGB(44, 35, 219)} };
				int ringColorIndex = 0;
				int centerColorIndex = 0;

				if (Piece::isRed(piece)) {
					ringColorIndex = 2;
				}
				else if (Piece::isBlue(piece)) {
					ringColorIndex = 3;
				}

				if (Piece::isAddOn(piece)) {
					centerColorIndex = ringColorIndex;
				}
				else if (Piece::isWhite(piece)) {
					centerColorIndex = 1;
					if (ringColorIndex == 0) ringColorIndex = 1;
				}


				HBRUSH hbr = CreateSolidBrush(colors[ringColorIndex][1]);
				HBRUSH hOld = (HBRUSH)SelectObject(hdc, hbr);


				Ellipse(hdc, 
					boardPosition.x + (i + piecePadding) * cellSize, 
					boardPosition.y + (j + piecePadding) * cellSize,
					boardPosition.x + (i + 1 - piecePadding) * cellSize,
					boardPosition.y + (j + 1 - piecePadding) * cellSize);
				hbr = CreateSolidBrush(colors[centerColorIndex][0]);
				hOld = (HBRUSH)SelectObject(hdc, hbr);
				Ellipse(hdc, 
					boardPosition.x + (i + piecePadding + pieceBorder) * cellSize,
					boardPosition.y + (j + piecePadding + pieceBorder) * cellSize,
					boardPosition.x + (i + 1 - piecePadding - pieceBorder) * cellSize, 
					boardPosition.y + (j + 1 - piecePadding - pieceBorder) * cellSize);

				SelectObject(hdc, hOld);
				DeleteObject(hbr);

				if (Piece::isAddOn(piece)) continue;

				SetRect(&rect, 
					boardPosition.x + i * cellSize,
					boardPosition.y + (j + 0.35) * cellSize,
					boardPosition.x + (i + 1) * cellSize,
					boardPosition.y + (j + 0.7) * cellSize);
				SetTextColor(hdc, colors[(centerColorIndex + 1) % 2][0]);
				SetBkColor(hdc, colors[centerColorIndex][0]);


				std::string str = std::to_string(Piece::height(piece));
				std::string temp = std::string(str.begin(), str.end());
				LPCSTR wideString = temp.c_str();
				//DrawText(hdc, wideString, -1, &rect, DT_CENTER);

			}

			// Highlights -- Draw yellow borders around highlighted cells
			if (cellHighlights[0][i][j])
			{
				HBRUSH yellowBrush = CreateSolidBrush(RGB(255, 255, 0));

				// Compute the rectangle for the clicked cell
				RECT cellRect = {
					boardPosition.x + i * cellSize,
					boardPosition.y + j * cellSize,
					boardPosition.x + (i + 1) * cellSize,
					boardPosition.y + (j + 1) * cellSize
				};

				// Draw yellow border
				FrameRect(hdc, &cellRect, yellowBrush);
				DeleteObject(yellowBrush);
			}

		}
	}

	EndPaint(hwnd, &ps);
}

void UI::setHighlight(int x, int y)
{
	cellHighlights[1][x][y] = cellHighlights[0][x][y];
	cellHighlights[0][x][y] = true;
}
void UI::clearHighlight(int x, int y)
{
	cellHighlights[1][x][y] = cellHighlights[0][x][y];
	cellHighlights[0][x][y] = false;
}
void UI::clearAllHighlights()
{
	for (int x = 0; x < 13; x++) {
		for (int y = 0; y < 13; y++) {
			cellHighlights[1][x][y] = cellHighlights[0][x][y];
			cellHighlights[0][x][y] = false;
		}
	}
}
