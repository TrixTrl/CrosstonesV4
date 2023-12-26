#ifndef UNICODE
#define UNICODE
#endif 

#include <windows.h>
#include <WinUser.h>
#include <wchar.h>

#include "BoardState.h"
#include "Globals/Piece.h"
#include "GameMaster.h"
#include "Test Bots/TestBotRnd.h"
#include "Felix Bots/Deepchad.h"
#include "Trix Bots/TheFirst.h"
#include "Trix Bots/Utils.h"

#include <bitset>
#include <thread>
#include <string>
#include <chrono>

#define KEYBOARDCONTROLL 1
#define EXPLOREMOVEGENERATION 0
#define EVALUATIONTESTING 0

int screenWidth = 750;//1200;
int screenHeight = 750;

uint8_t displayBoard[3][13][13];
HWND globalHwnd = NULL;
bool firstDraw = true;
int switchMove = 0;

int main() {
	srand(time(nullptr));

	if (EXPLOREMOVEGENERATION || EVALUATIONTESTING) {
		BoardState bs;
		std::bitset<3> set(5);
		bs.rst(set);

		//bs.loadPos("b-10002 b-10012 -W10104 -B20204 r-10207 -B40400 -W10410 -W10607 -W10609 -B10708 -W10812 b-11200 -B11202 r-11206 b-11212 111111011110100111111011");
		bs.loadPos("-B10700 -W10803 -W10602 00000000100000000000");
		
		bs.copyBoard(&(displayBoard[0]));

		InvalidateRect(globalHwnd, NULL, NULL);

		bool isWhite = true;
		std::shared_ptr<std::vector<std::vector<BoardState::xMove>>> moves;
		moves = bs.getMoves(isWhite);

		std::string str = std::to_string(moves->size());
		str += "\n";
		std::wstring temp = std::wstring(str.begin(), str.end());
		LPCWSTR wideString = temp.c_str();
		OutputDebugString(wideString);


		//Timing testing code

		/*std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
		for (int i = 0; i < 100000; i++) {
			bs.getMoves(i%2);
		}
		std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);

		std::string strT = std::to_string(time_span.count());
		strT += "\n";
		std::wstring tempT = std::wstring(strT.begin(), strT.end());
		LPCWSTR wideStringT = tempT.c_str();
		OutputDebugString(wideStringT);

		return 0;*/


		//Cycle through moves with left and right arrow keys
		//Up and down arrows incrent and decrement the counter by 10 respectively
		//The delete key (entf) resets back to 0

		int i = 0;			//--------------------------------------------------------------
		while (true) {
			std::this_thread::sleep_for(std::chrono::milliseconds(50));

			bs.makeMove(&((*moves)[i % moves->size()]), isWhite);
			bs.copyBoard(&(displayBoard[0]));
			bs.unsafeMakeMove(&((*moves)[i % moves->size()]));

			Utils::print("--------", true);

			std::string str2 = EVALUATIONTESTING?std::to_string(Utils::basicPosEval(isWhite, &(displayBoard[0]))):std::to_string(i);
			str2 += "\n";
			std::wstring temp2 = std::wstring(str2.begin(), str2.end());
			LPCWSTR wideString2 = temp2.c_str();
			OutputDebugString(wideString2);

			Utils::debugContainer debug;
			Utils::print(Utils::alphaBeta(&(displayBoard[0]), 4, -std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(), isWhite, Utils::basicPosEval, &debug), true);
			Utils::print(debug.n, true);
			Utils::print(debug.depthCounts[4], true);
			Utils::print(debug.depthCounts[3], true);
			Utils::print(debug.depthCounts[2], true);
			Utils::print(debug.depthCounts[1], true);
			Utils::print(debug.depthCounts[0], true);

			InvalidateRect(globalHwnd, NULL, NULL);
			if (!KEYBOARDCONTROLL) {
				i++;
				std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			}
			else {
				while (switchMove == 0) std::this_thread::sleep_for(std::chrono::milliseconds(100));
				if (switchMove != -99999) {
					i += moves->size() * 3 + switchMove;
					i %= moves->size();
				}
				else {
					i = 0;
				}
				switchMove = 0;
			}
		}

	}
	else {
		Player* p1 = new TheFirst();
		Player* p2 = new TheFirst();
		std::bitset<3> gamemode(5);
		GameMaster gameMaster(gamemode, p1, p2, 3000, 0, &(displayBoard[0]));

		gameMaster.play(globalHwnd);
	}
	return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	memset(displayBoard, 0, sizeof(displayBoard));

	// Register the window class.
	const wchar_t CLASS_NAME[] = L"Sample Window Class";

	WNDCLASS wc = { };

	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;

	RegisterClass(&wc);

	// Create the window.

	HWND hwnd = CreateWindowEx(
		0,                              // Optional window styles.
		CLASS_NAME,                     // Window class
		L"Crosstones V4",    // Window text
		WS_OVERLAPPED | WS_MINIMIZEBOX | WS_SYSMENU,            // Window style

		// Size and position
		CW_USEDEFAULT, CW_USEDEFAULT, screenWidth + 15, screenHeight + 39,

		NULL,       // Parent window    
		NULL,       // Menu
		hInstance,  // Instance handle
		NULL        // Additional application data
	);

	if (hwnd == NULL)
	{
		return 0;
	}

	ShowWindow(hwnd, nCmdShow);

	// Run the message loop.

	std::thread mainThread(main);
	mainThread.detach();

	MSG msg = { };
	while (GetMessage(&msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	wchar_t msg[32];

	switch (uMsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_PAINT:
	{
		//OutputDebugString(L"Painting\n");

		memcpy(displayBoard[2], displayBoard[1], sizeof(displayBoard[0]));
		memcpy(displayBoard[1], displayBoard[0], sizeof(displayBoard[0]));


		/*std::string str = std::to_string((int)displayBoard[1][6][0]);
		str += "\n";
		std::wstring temp = std::wstring(str.begin(), str.end());
		LPCWSTR wideString = temp.c_str();
		OutputDebugString(wideString);*/

		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);

		// All painting occurs here, between BeginPaint and EndPaint.

		if (firstDraw) {
			HBRUSH background = CreateSolidBrush(RGB(0, 0, 255));
			FillRect(hdc, &ps.rcPaint, background);		//Background
		}

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
		float padding = 20;
		float sideLength = (screenHeight - padding * 2) / 13;

		enum DrawPath
		{
			NONE = 0, HORIZONTAL, VERTICAL
		};

		//SetRect(&rect, 0, 650, 100, 750);
		//FillRect(hdc, &rect, (HBRUSH)(CreateSolidBrush(RGB(255, 0, 0))));
		//DrawText(hdc, L"Hello World!", -1, &ps.rcPaint, DT_CENTER);

		for (int i = 0; i < 13; i++) {
			for (int j = 0; j < 13; j++) {
				//if (!(i == 2 && j == 2)) {
				if (!firstDraw && displayBoard[1][i][j] == displayBoard[2][i][j]) continue;
				//}
				SetRect(&rect, padding + i * sideLength, padding + j * sideLength, padding + (i + 1) * sideLength, padding + (j + 1) * sideLength);
				HBRUSH col;
				DrawPath path = NONE;
				uint8_t piece = displayBoard[1][i][j];		//------------------------------------------------------------------
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
					path = (piece & BoardState::setTurnPiece) == 0 ? DrawPath::HORIZONTAL : DrawPath::VERTICAL;
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
					rect.top += sideLength / 3;
					rect.bottom -= sideLength / 3;
					col = CreateSolidBrush(RGB(63, 45, 35));
					FillRect(hdc, &rect, col);
					break;
				case DrawPath::VERTICAL:
					rect.left += sideLength / 3;
					rect.right -= sideLength / 3;
					col = CreateSolidBrush(RGB(63, 45, 35));
					FillRect(hdc, &rect, col);
					break;
				}

				if ((piece & 0b00111111) != 0) {
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


					Ellipse(hdc, padding + (i + piecePadding) * sideLength, padding + (j + piecePadding) * sideLength, padding + (i + 1 - piecePadding) * sideLength, padding + (j + 1 - piecePadding) * sideLength);
					hbr = CreateSolidBrush(colors[centerColorIndex][0]);
					hOld = (HBRUSH)SelectObject(hdc, hbr);
					Ellipse(hdc, padding + (i + piecePadding + pieceBorder) * sideLength, padding + (j + piecePadding + pieceBorder) * sideLength, padding + (i + 1 - piecePadding - pieceBorder) * sideLength, padding + (j + 1 - piecePadding - pieceBorder) * sideLength);

					SelectObject(hdc, hOld);
					DeleteObject(hbr);

					if (Piece::isAddOn(piece)) continue;

					SetRect(&rect, padding + i * sideLength, padding + (j + 0.35) * sideLength, padding + (i + 1) * sideLength, padding + (j + 0.7) * sideLength);
					SetTextColor(hdc, colors[(centerColorIndex + 1) % 2][0]);
					SetBkColor(hdc, colors[centerColorIndex][0]);


					std::string str = std::to_string(Piece::height(piece));
					std::wstring temp = std::wstring(str.begin(), str.end());
					LPCWSTR wideString = temp.c_str();
					DrawText(hdc, wideString, -1, &rect, DT_CENTER);

				}

			}
		}

		EndPaint(hwnd, &ps);
		firstDraw = false;
	}
	return 0;

	case WM_SYSKEYDOWN:
		break;
		swprintf_s(msg, L"WM_SYSKEYDOWN: 0x%x\n", (wchar_t)wParam);
		OutputDebugString(msg);
		break;

	case WM_SYSCHAR:
		break;
		swprintf_s(msg, L"WM_SYSCHAR: %c\n", (wchar_t)wParam);
		OutputDebugString(msg);
		break;

	case WM_SYSKEYUP:
		break;
		swprintf_s(msg, L"WM_SYSKEYUP: 0x%x\n", (wchar_t)wParam);
		OutputDebugString(msg);
		break;

	case WM_KEYDOWN:
		//swprintf_s(msg, L"WM_KEYDOWN: 0x%x\n", (wchar_t)wParam);
		//outputDebugString(msg);
		if ((wchar_t)wParam == 0x20) {
			firstDraw = true;
			InvalidateRect(globalHwnd, NULL, NULL);
		}
		if (KEYBOARDCONTROLL) {
			if ((wchar_t)wParam == 0x27) { switchMove = 1; }
			if ((wchar_t)wParam == 0x25) { switchMove = -1; }
			if ((wchar_t)wParam == 0x26) { switchMove = 10; }
			if ((wchar_t)wParam == 0x28) { switchMove = -10; }
			if ((wchar_t)wParam == 0x2E) { switchMove = -99999; }
		}
		break;

	case WM_KEYUP:
		break;
		swprintf_s(msg, L"WM_KEYUP: 0x%x\n", (wchar_t)wParam);
		OutputDebugString(msg);
		break;

	case WM_CHAR:
		break;
		swprintf_s(msg, L"WM_CHAR: %c\n", (wchar_t)wParam);
		OutputDebugString(msg);
		break;

	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}