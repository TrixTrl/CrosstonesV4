#include "GameMaster.h"
#include <Windows.h>
#include <WinUser.h>

void print(std::string str) {
	std::wstring temp = std::wstring(str.begin(), str.end());
	LPCWSTR wideString = temp.c_str();
	OutputDebugString(wideString);
}

GameMaster::GameMaster(std::bitset<3>& gamemode, Player* player1, Player* player2, int timeControl_, int enforceTime, uint8_t (*displayBoard)[13][13]) {
	bs.rst(gamemode);
	players[0] = player1;
	players[1] = player2;
	timeControl = timeControl_;
	timeEnforcement[0] = (enforceTime & 2) != 0;
	timeEnforcement[1] = (enforceTime & 1) != 0;
	displayBoardPointer = displayBoard;
}

void GameMaster::play(HWND globalHwnd) {
	bs.copyBoard(displayBoardPointer);
	InvalidateRect(globalHwnd, NULL, NULL);

	bool ended = false;
	bool passed[2] = { false, false };	//we keep track of if the last move of either player was passing
	while (bs.gameOver(!isWhiteTurn) == BoardState::winValue::none && !ended) {		//we only check for wins after the move was made and the playing color was switched, so our win check needs the inverse value
		uint8_t board[13][13];
		bs.copyBoard(&board);
		int timeEnd = Time::millis() + timeControl;
		(*players[isWhiteTurn ? 0 : 1]).getMoveToPlay(&board, isWhiteTurn, timeEnd);
		if (timeEnforcement[isWhiteTurn ? 0 : 1] && timeEnd < Time::millis()) {
			ended = true;
			print("\n");
			print(isWhiteTurn ? "White" : "Black");
			print(" has lost by going over the allotted time\n");
		}
		else {
			int feedback;
			feedback = bs.makeMove(&board, isWhiteTurn);
			bs.copyBoard(displayBoardPointer);
			InvalidateRect(globalHwnd, NULL, NULL);
			if (feedback == -1) {
				ended = true;
				print("\n");
				print(isWhiteTurn ? "White" : "Black");
				print(" has lost due to an invalid move\n");
			}
			else if (feedback == 0) {
				if (passed[!isWhiteTurn]) {
					ended = true;
					print("\nDraw due to double passing\n");
				}
				else if (passed[isWhiteTurn]) {
					ended = true;
					print("\n");
					print(isWhiteTurn ? "White" : "Black");
					print(" has lost due to resigning through double passing\n");
				}
				else {
					passed[isWhiteTurn] = true;
				}
			}
			else {
				passed[isWhiteTurn] = false;
			}
		}
		isWhiteTurn = !isWhiteTurn;
	}
	if (!ended) {
		BoardState::winValue gameOver = bs.gameOver(isWhiteTurn);
		print("\n");
		print(bs.gameOver(isWhiteTurn) == BoardState::winValue::white ? "White" : "Black");
		print(" has won\n");
	}
}