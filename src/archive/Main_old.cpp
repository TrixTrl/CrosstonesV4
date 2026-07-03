#ifndef UNICODE
#define UNICODE
#endif

#include "fix_win32_compatibility.h"
#include <WinUser.h>
#include <wchar.h>
#include <windowsx.h>

#include "NewUI.h"

#include "DCTestSuite.h"
#include "Board.h"
#include "globals/Piece.h"
#include "GameMaster.h"
#include "ManualPlayer.h"
#include "test-bots/TestBotRnd.h"
#include "felix-bots/Deepchad.h"
#include "trix-bots/TheFirst.h"
#include "trix-bots/Utils.h"
#include "trix-bots/DemoKnight.h"
#include "trix-bots/Hydra.h"
#include "trix-bots/AlphaCruncher.h"
#include "trix-bots/GeneratorComparer.h"

#include "globals/PlayerInputKey.h"

#include <bitset>
#include <thread>
#include <string>
#include <chrono>
#include "UI.h"

#define CUSTOM_STACK_SIZE 67108864  // 64MB - do not overflow 32-bit integer limit, else ignored
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

#define KEYBOARDCONTROLL 0
#define EXPLOREMOVEGENERATION 0
#define EXPLORECHADMOVES 0
#define EVALUATIONTESTING 0
#define DEEPCHADTESTINGSUITE 0
#define GAMEVIEWER 0
#define RUNNINGCOMPARISON 1
#define RUNNINGTIMECOMPARISON 0

const int screenWidth = 755;
const int screenHeight = 755;

GamePosition displayBoard[3];
HWND globalHwnd = NULL;

GameMaster *gameMaster = nullptr;
UI ui = UI();

bool firstDraw = true;
PlayerInputKey playerInputKey = PlayerInputKey::None;
int switchMove = 0;

int main()
{
	srand(time(nullptr));

	if (EXPLOREMOVEGENERATION || EVALUATIONTESTING)
	{
		Board bs;
		std::bitset<3> set(5);
		bs.rst(set);

		// bs.loadPos("b-10000 b-10012 b-11200 b-11212 b-10606 r-10006 r-11206 -W30512 -W30510 -B30502 -B30500 -W30712 -W30710 -B30702 -B30700 11111111111111111111"); //Dragon start
		// bs.loadPos("-B30500 b-11200 bW20204 -W10304 -B30804 -B11204 -B20410 -W11210 bW20212 r-10412 -W10512 -W20712 11110111110111111010"); //"unsolved" endgame
		// bs.loadPos("b-10000 b-10012 rW30406 -W30512 -W10602 -W10603 -B10700 r-10704 -W30712 -B20806 -B10906 b-11200 b-11212 11110111111101111111");
		// bs.loadPos("b-10002 b-10012 -W10104 -B20204 r-10207 -B40400 -W10410 -W10607 -W10609 -B10708 -W10812 b-11200 -B11202 r-11206 b-11212 111111011110100111111011");
		// bs.loadPos("b-10000 b-10012 b-11200 b-11212 rW20006 r-11206 b-10608 -B10206 -B20204 -B30500 -B30700 -W30512 -W30712 -W10605 -W10607 -W10808 -W20809 -B21104 11110111111111111111"); //Seagull (black)
		// bs.loadPos("-B10700 -W10803 -W10602 00000000000000000000");
		bs.loadPosition("b-10000 rB20006 b-10012 -B30300 -B20404 -W20408 -W30512 -W20602 bW20605 -B30804 -W10805 -W30912 b-11200 rB21206 b-11212 11111111111111111111");
		bool isWhite = false;

		// bs.loadPos("-B10800 -W10803 -W10602 00000000000000000000");
		// bs.loadPos("-B10500 -B10600 -B10700 -B10501 -B10601 -B10701 -B10502 -B10602 -B10702 -B10503 -B10603 -B10703 -W10509 -W10609 -W10709 -W10510 -W10610 -W10710 -W10511 -W10611 -W10711 -W10512 -W10612 -W10712 00000000000000000000");
		// bs.loadPos("-W10909 -W11209 -W10910 -W11210 -W10911 -W11211 -W10912 -W11212 00000000000000000000");
		// bs.loadPos("-W10009 -W10309 -W10010 -W10310 -W10011 -W10311 -W10012 -W10312 00000000000000000000");
		bs.copyPositionTo(displayBoard[0]);

		// Utils::print(Utils::generateMetadata(&(displayBoard[0])), false);

		InvalidateRect(globalHwnd, NULL, NULL);

		std::vector<std::vector<Board::xMove>> moves = bs.getMoves(isWhite);

		std::string str = std::to_string(moves.size());
		str += "\n";
		std::wstring temp = std::wstring(str.begin(), str.end());
		LPCWSTR wideString = temp.c_str();
		OutputDebugString(wideString);

		// Timing testing code

		/*std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
		for (int i = 0; i < 100000; i++) {
			bs.getMoves(i%2);
		}
		std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);

		std::string strT = std::to_string(time_span.count());
		strT += "\n";
		std::string tempT = std::string(strT.begin(), strT.end());
		LPCSTR wideStringT = tempT.c_str();
		OutputDebugString(wideStringT);

		return 0;*/

		// Prepare chad move exploration
		dc::BotBoard chadBoard = dc::BotBoard();
		chadBoard.initialize(&displayBoard[0].pieces, isWhite);

		std::vector<dc::Move> chadMoves;
		dc::Evaluation evaluator = dc::Evaluation();
		dc::MoveGenerator::getMovesStatic(&chadMoves, &displayBoard[0].pieces, isWhite, true);
		std::vector<int> chadMoveIndices = MoveOrdering(evaluator).makeMoveOrdering(Utility::createNullMove(), chadBoard, chadMoves, true);

		// Cycle through moves with left and right arrow keys
		// Up and down arrows incrent and decrement the counter by 10 respectively
		// The delete key (entf) resets back to 0

		int i = 0; //--------------------------------------------------------------
		while (true)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
			if (EXPLORECHADMOVES && chadMoves.size() > 0)
			{
				chadBoard.makeMove(chadMoves[chadMoveIndices[i % chadMoves.size()]]);
				memcpy(&displayBoard[0], chadBoard.square, sizeof(displayBoard[0]));
				chadBoard.unmakeMove(chadMoves[chadMoveIndices[i % chadMoves.size()]]);
				/*Utils::print(std::format(
						"\nShowing chad move at: {} / {}, ( trueIndex: {} )",
						i % chadMoves.size(),
						chadMoves.size(),
						chadMoveIndices[i % chadMoves.size()]
					), false);*/
			}
			else
			{
				bs.makeMove(moves[i % moves.size()], isWhite);
				bs.copyPositionTo(displayBoard[0]);
				bs.unsafeMakeMove(moves[i % moves.size()]);
			}
			InvalidateRect(globalHwnd, NULL, NULL);

			Utils::print("--------", true);

			std::string str2 = EVALUATIONTESTING ? std::to_string(Utils::improvedPosEval(isWhite, &(displayBoard[0].pieces))) : std::to_string(i);
			str2 += "\n";
			std::wstring temp2 = std::wstring(str2.begin(), str2.end());
			LPCWSTR wideString2 = temp2.c_str();
			OutputDebugString(wideString2);
			// Trix
			Utils::debugContainer debug;
			Utils::print("First: ", false);
			// Utils::print(Utils::alphaBeta(&(displayBoard[0]), 2, -std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(), isWhite, Utils::basicPosEval, 0, &debug), true);
			Utils::print(debug.n, true);
			Utils::print(debug.depthCounts[4], true);
			Utils::print(debug.depthCounts[3], true);
			Utils::print(debug.depthCounts[2], true);
			Utils::print(debug.depthCounts[1], true);
			Utils::print(debug.depthCounts[0], true);

			// Felix
			Deepchad chad = Deepchad(1);

			Utils::print("Chad: ", false);
			GamePosition dcTempBoard = displayBoard[0];
			chad.getMoveToPlay(&dcTempBoard.pieces, !isWhite, 1000);

		EXPLORE_WAIT_FOR_UPDATE:
			InvalidateRect(globalHwnd, NULL, NULL);
			if (!KEYBOARDCONTROLL)
			{
				i++;
				std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			}
			else
			{
				while (switchMove == 0)
					std::this_thread::sleep_for(std::chrono::milliseconds(100));
				// reset move
				if (switchMove == -99999)
				{
					i = 0;
				}
				// Show the move that DC would have played for the opponent (on second call undo it)
				else if (switchMove == -12345)
				{
					GamePosition copiedBoard = displayBoard[0];
					displayBoard[0] = dcTempBoard;
					dcTempBoard = copiedBoard;
					switchMove = 0;
					goto EXPLORE_WAIT_FOR_UPDATE;
				}
				else
				{
					i += moves.size() * 3 + switchMove;
					i %= moves.size();
				}
				switchMove = 0;
			}
		}
	}
	else if (DEEPCHADTESTINGSUITE)
	{
		DCTestSuite::run(globalHwnd, &displayBoard[0].pieces);
	}
	else if (GAMEVIEWER)
	{
		// std::string gameCode = "b-10000 r-10006 b-10012 -B30500 -B30502 -W30510 -W30512 b-10606 -B30700 -B30702 -W30710 -W30712 b-11200 r-11206 b-11212 11111111111111111111 | 41 63 93 69 104 223 100 90 126 19 92 43 120 115 175 23 102 26 169 41 177 117 144 134 115 180 97 93 66 209 ";
		/*2 Hydras with Felix eval*/					 // std::string gameCode = "b-10000 r-10006 b-10012 -B30500 -B30502 -W30510 -W30512 b-10606 -B30700 -B30702 -W30710 -W30712 b-11200 r-11206 b-11212 11111111111111111111 | 29 132 75 63 186 255 32 290 38 276 37 128 67 222 129 266 103 23 192 166 120 154 149 194 143 150 ";
		/*Hydra w/ dc eval depth 3 vs Deepchad depth 5*/ // std::string gameCode = "b-10000 r-10006 b-10012 -B30500 -B30502 -W30510 -W30512 b-10606 -B30700 -B30702 -W30710 -W30712 b-11200 r-11206 b-11212 11111111111111111111 | 95 61 247 156 69 53 19 62 28 21 245 51 208 52 298 29 181 109 84 340 54 44 85 223 139 71 47 2 1 28 0 263 0 ";
		/*2 Hydras with fixed Felix eval*/				 // std::string gameCode = "b-10000 r-10006 b-10012 -B30500 -B30502 -W30510 -W30512 b-10606 -B30700 -B30702 -W30710 -W30712 b-11200 r-11206 b-11212 11111111111111111111 | 29 132 216 61 259 52 247 27 84 74 10 112 336 138 60 110 53 79 30 78 455 16 424 274 216 140 144 181 215 186 175 99 22 56 88 105 9 129 143 58 145 242 1 68 184 145 53 165 17 91 13 72 118 1 91 1 125 3 129 3 75 32 75 49 ";
		/*Hydra getting executed by DC*/				 // std::string gameCode = "b-10000 r-10006 b-10012 -B30500 -B30502 -W30510 -W30512 b-10606 -B30700 -B30702 -W30710 -W30712 b-11200 r-11206 b-11212 11111111111111111111 | 95 63 247 253 67 162 29 112 224 128 5 121 12 183 100 61 22 193 64 227 22 216 ";
		/*DeepChad delth 5 mirror*/						 // std::string gameCode = "b-10000 r-10006 b-10012 -B30500 -B30502 -W30510 -W30512 b-10606 -B30700 -B30702 -W30710 -W30712 b-11200 r-11206 b-11212 11111111111111111111 | 27 132 12 134 24 112 163 140 6 63 91 274 67 488 374 397 131 83 180 36 47 147 124 100 26 22 22 62 36 205 6 139 202 43 233 57 1 18 291 9 230 204 161 99 39 147 288 114 205 143 291 25 353 78 295 124 182 59 262 121 219 160 98 103 51 69 260 165 26 29 296 8 4 16 42 7 34 26 151 43 21 131 107 113 180 92 127 140 77 117 139 167 51 30 62 65 222 131 206 40 252 25 48 32 26 4 164 53 68 3 118 10 72 21 92 10 132 27 156 29 134 6 135 22 144 55 74 40 134 43 134 41 135 22 144 55 74 40 134 43 134 41 135 22 144 55 74 40 134 43 134 41 135 22 144 55 74 40 134 43 134 41 135 22 144 55 74 40 134 43 134 41 135 22 144 55 74 40 134 43 134 41 135 22 144 55 74 40 134 43 134 41 135 22 144 55 74 40 134 43 134 41 135 22 144 55 74 40 134 43 134 41 135 22 ";
		/*Man vs AI (Longmen Insanity)*/				 // std::string gameCode = "b-10000 r-10006 b-10012 -B30500 -B30502 -W30510 -W30512 b-10606 -B30700 -B30702 -W30710 -W30712 b- 11200 r-11206 b-11212 11111111111111111111 | 27 132 12 39 134 176 24 50 26 15 45 154 51 133 114 237 542 491 151 165 19 23 25 184 341 351 451 427 168 429 449 318 464 364 308 30 216 181 177 457 415 574 158 619 66 386 413 364 3 271 24 292 87 278 186 12 258 71 96 83 38 33 93 19 191 211 54 251 209 219 261 263 94 262 79 224 218 289 206 239 184 182 27 2 135 256 86 202 53 241 12 240";
		/*DC(white) vs v1 of AlphaCruncher*/ std::string gameCode = "b-10000 r-10006 b-10012 -B30500 -B30502 -W30510 -W30512 b-10606 -B30700 -B30702 -W30710 -W30712 b-11200 r-11206 b-11212 11111111111111111111 | 20 95 17 151 141 29 37 125 35 175 148 6 29 102 105 163 73 108 105 250 16 127 111 179 29 312 29 103 28 163";

		Board board;

		size_t pos = gameCode.find("|");
		std::string startingPos = gameCode.substr(0, pos - 1);
		board.loadPosition(startingPos);
		board.copyPositionTo(displayBoard[0]);
		InvalidateRect(globalHwnd, NULL, NULL);

		gameCode.erase(0, pos + 2);

		size_t pos2 = 0;
		std::vector<std::vector<Board::xMove>> gameReplay;
		// gameReplay = std::vector<std::vector<BoardState::xMove>>();

		while ((pos2 = gameCode.find(" ")) != std::string::npos)
		{
			int moveIndex = std::stoi(gameCode.substr(0, pos2));
			gameCode.erase(0, pos2 + 1);

			std::vector<std::vector<Board::xMove>> moves = board.getMoves(gameReplay.size() % 2 == 0);
			std::vector<Board::xMove> move = moves[moveIndex];

			gameReplay.emplace_back(move);
			board.makeMove(gameReplay[gameReplay.size() - 1], gameReplay.size() % 2 == 1);
		}
		board.loadPosition(startingPos);
		int currentMove = 0;
		while (true)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
			while (switchMove == 0)
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
			if (switchMove == 1)
			{
				switchMove = 0;
				if (currentMove == gameReplay.size())
					continue;
				board.unsafeMakeMove(gameReplay[currentMove]);
				board.copyPositionTo(displayBoard[0]);
				InvalidateRect(globalHwnd, NULL, NULL);
				currentMove++;
				Utils::print(board.dumpPosition(), true);
			}
			else if (switchMove == -1)
			{
				switchMove = 0;
				if (currentMove == 0)
					continue;
				board.unsafeMakeMove(gameReplay[currentMove - 1]);
				board.copyPositionTo(displayBoard[0]);
				InvalidateRect(globalHwnd, NULL, NULL);
				currentMove--;
				Utils::print(board.dumpPosition(), true);
			}
			else if (switchMove == -99999)
			{
				switchMove = 0;
				while (currentMove > 0)
				{
					board.unsafeMakeMove(gameReplay[currentMove - 1]);
					currentMove--;
				}
				board.copyPositionTo(displayBoard[0]);
				InvalidateRect(globalHwnd, NULL, NULL);
				Utils::print(board.dumpPosition(), true);
			}
			else
			{
				switchMove = 0;
			}
		}
	}
	else if (RUNNINGCOMPARISON)
	{
		compareGenerators();
	}
	else if (RUNNINGTIMECOMPARISON)
	{
		executionSpeedTest();
	}
	else
	{
		Player *p1 = new
			// TheFirst(2);
			// Deepchad(4);
			// Hydra(4, 14);
			// ManualPlayer(&ui, globalHwnd, &displayBoard[0]);
			AlphaCruncher(60);
		Player *p2 = new
			// TheFirst(2);
			// Hydra(4, 14);
			// Deepchad(4);
			AlphaCruncher(60);

		std::bitset<3> gamemode(0b010); /*Gates, Bases, Ports*/
		gameMaster = new GameMaster(gamemode, p1, p2, 10000, 0, displayBoard[0]);

		// gameMaster->loadPos("b-10000 b-10012 -B30404 -W30512 -B30602 rW30608 bB50610 -W30712 rW40808 b-11200 b-11212 11111111100111111111");
		// gameMaster->loadPos("b-10000 b-10012 -B30404 -W30512 -B30602 rW50607 bB50610 -W30712 rW20808 b-11200 b-11212 11111111100111111111");
		// gameMaster->loadPos("-W10006 -B11206 11010100001100111111");
		// gameMaster->loadPos("-W20006 -B21206 11010100001100111111");
		// gameMaster->loadPos("bW30404 -B10600 -B10800 rW30804 11010100001100111111");
		// gameMaster->loadPos("b-10000 b-10012 rW30406 -B30500 -B30502 -W10512 rB20610 bB30611 -B30700 -W30712 b-11200 b-11212 11110011111111101111");
		// gameMaster->loadPos("b-10000 r-10006 b-10012 -W30408 -B30500 -B30502 -W30512 b-10606 -B30700 -B30702 -W30710 -W30712 b-11200 r-11206 b-11212 11111111111111111111");
		// gameMaster->loadPos("b-10000 r-10006 b-10012 -B10500 -B10502 -W10510 -W10512 b-10606 -B10700 -B10702 -W10710 -W10712 b-11200 r-11206 b-11212 11111111111111111111");
		// gameMaster->loadPos("b-10000 r-10006 b-10012 -W10409 -B10502 bW20603 -W20710 -B10803 -B10900 b-11200 r-11206 b-11212 11111111111111111111");
		// gameMaster->loadPos("-B10500 -W10602 11111111111111111111");
		// gameMaster->loadPos("b-10000 r-10006 b-10012 -W30510 -B30502 b-10606 -B30702 -W30710 b-11200 r-11206 b-11212 11111111111111111111"); // slaughterhouse
		// gameMaster->loadPos("b-10000 r-10006 b-10012 -W30609 -B30603 b-11200 r-11206 b-11212 11111111111111111111");
		gameMaster->loadPos("b-10002 b-10005 -B10102 -W20308 rW30404 -B10500 -B10512 -B10602 -W20604 b-10607 r-10608 -W30610 -B10700 b-10708 -W10804 -W10912 -B11100 -B11110 11111100101101111111");
		// gameMaster->loadPos("-B50506 -B50605 rW30606 -W10607 -B50706 11111100101101111111");
		/// gameMaster->loadPos("r-10006 bW40112 -B30202 -B20300 -B10302 bB20400 -B10402 -W30411 -W20508 bW20606 -W30610 -B20801 -B20902 b-11200 r-11206 b-11212 00000000000100000000");
		RaylibUI::init();
		std::jthread gm_thread([](std::stop_token stopToken) {
			gameMaster->play(stopToken, globalHwnd, true);
		});


		RaylibUI::draw(&displayBoard[0].pieces);

		// gameMaster->play(globalHwnd, /*whiteToStart: */ false);
		//   while(true) {}
	}
	return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

DWORD WINAPI MainThreadWrapper(LPVOID lpParam) {
    main();
    return 0;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	memset(displayBoard, 0, sizeof(displayBoard));

	// Register the window class.
	const wchar_t CLASS_NAME[] = L"Sample Window Class";

	WNDCLASS wc = {};

	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;

	RegisterClass(&wc);

	// Create the window.

	HWND hwnd = CreateWindowEx(
		0,											 // Optional window styles.
		CLASS_NAME,									 // Window class
		L"Crosstones V4",							 // Window text
		WS_OVERLAPPED | WS_MINIMIZEBOX | WS_SYSMENU, // Window style

		// Size and position
		CW_USEDEFAULT, CW_USEDEFAULT, screenWidth + 15, screenHeight + 39,

		NULL,	   // Parent window
		NULL,	   // Menu
		hInstance, // Instance handle
		NULL	   // Additional application data
	);

	if (hwnd == NULL)
	{
		return 0;
	}

	ShowWindow(hwnd, nCmdShow);

	HANDLE hThread = CreateThread(
		NULL,                               // Default security attributes
		CUSTOM_STACK_SIZE,                  // forced 16MB stack size
		MainThreadWrapper,                  // Thread function
		NULL,                               // Argument
		STACK_SIZE_PARAM_IS_A_RESERVATION,  // Let it grow up to 16MB
		NULL                                // Don't need thread ID
	);

	if (hThread != NULL) {
		CloseHandle(hThread);
	}

	MSG msg = {};
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
		displayBoard[2] = displayBoard[1];
		displayBoard[1] = displayBoard[0];

		ui.paint(hwnd, firstDraw, &displayBoard[1].pieces);

		firstDraw = false;
		return 0;
	}

	case WM_LBUTTONDOWN:
	{ // handle left mouse click
		POINT pos = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
		POINT gridPos = ui.lclick(hwnd, pos);

		// Notify player controller
		if (gameMaster != nullptr)
			gameMaster->notifyPlayersClicked(true, gridPos);

		break;
	}
		/*case WM_SYSKEYDOWN:
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
			break;*/

	case WM_KEYDOWN:
		// swprintf_s(msg, L"WM_KEYDOWN: 0x%x\n", (wchar_t)wParam);
		// outputDebugString(msg);
		if ((wchar_t)wParam == 'R')
		{
			firstDraw = true;
			InvalidateRect(globalHwnd, NULL, NULL);
		}
		if (KEYBOARDCONTROLL)
		{
			switch ((wchar_t)wParam)
			{
			case 'W':
			case VK_UP: // Process the UP ARROW key.
				switchMove = 10;
				playerInputKey = PlayerInputKey::Up;
				break;

			case 'A':
			case VK_LEFT: // Process the LEFT ARROW key.
				switchMove = -1;
				playerInputKey = PlayerInputKey::Left;
				break;

			case 'S':
			case VK_DOWN: // Process the DOWN ARROW key.
				switchMove = -10;
				playerInputKey = PlayerInputKey::Down;
				break;

			case 'D':
			case VK_RIGHT: // Process the RIGHT ARROW key.
				switchMove = 1;
				playerInputKey = PlayerInputKey::Right;
				break;

			case VK_DELETE: // Process the DEL key.
				switchMove = -99999;
				playerInputKey = PlayerInputKey::Undo;
				break;

			case '1':
				playerInputKey = PlayerInputKey::One;
				break;

			case '2':
				playerInputKey = PlayerInputKey::Two;
				break;

			case '3':
				playerInputKey = PlayerInputKey::Three;
				break;

			case '4':
				playerInputKey = PlayerInputKey::Four;
				break;

			case '5':
				playerInputKey = PlayerInputKey::Five;
				break;

			case ' ':
				switchMove = -12345;
				playerInputKey = PlayerInputKey::Turn;
				break;
			}
			// Notify player controller
			if (gameMaster != nullptr)
				gameMaster->notifyPlayersKeyDown(playerInputKey);
			playerInputKey = PlayerInputKey::None;
		}
		break;

	case WM_KEYUP:
		playerInputKey = PlayerInputKey::None;
		break;

	case WM_CHAR:
		if (KEYBOARDCONTROLL)
		{
			switch ((wchar_t)wParam)
			{
			case 0x08: // or '\b' // Process a backspace.
				playerInputKey = PlayerInputKey::Reset;
				break;

			case 0x09: // or '\t' // Process the TAB key.
				playerInputKey = PlayerInputKey::SwitchPushMergeMode;
				break;

			case 0x0D: // Process a carriage return / ENTER.
				playerInputKey = PlayerInputKey::Submit;
				break;
			}
			// Notify player controller
			if (gameMaster != nullptr)
				gameMaster->notifyPlayersKeyDown(playerInputKey);
			playerInputKey = PlayerInputKey::None;
		}
		break;
	default:
		break;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}