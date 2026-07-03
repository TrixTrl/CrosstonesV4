#pragma once
#include "globals/Player.h"
#include "fix_win32_compatibility.h"
#include <WinUser.h>
#include <stack>
#include "UI.h"

class ManualPlayer : public Player
{
public:
	ManualPlayer(UI* _ui, HWND _globalHwnd, uint8_t(*_displayBoard)[13][13]) 
		: originalDisplayBoardVal(), moveInfo(), prevStates()
	{
		ui = _ui;
		globalHwnd = _globalHwnd;
		targetBoard = nullptr;
		displayBoard = _displayBoard;
	}

	void getMoveToPlay(uint8_t(*board)[13][13], bool isWhite, int endTime);
	void keyDown(const PlayerInputKey key);
	void mouseDown(bool isLeft, POINT gridPos);

	bool listensToKeyInputs() override {
		return true;
	}

private:

	enum class Direction {
		Up,
		Right,
		Down,
		Left
	};


	// True while Player is inputting
	bool isActive = false;
	bool isWhite = false;

	// true if merge, false if push mode
	bool isMergeMode = true;

	uint8_t maxSteps = 0;
	// the *active* tower in it's beginning state
	uint8_t startTower = 0;

	//the board to which the final changes should be made.
	uint8_t(*targetBoard)[13][13];

	//access to display to show submoves
	uint8_t(*displayBoard)[13][13];
	HWND globalHwnd;
	UI* ui;

	//copy of display before modifying it
	uint8_t originalDisplayBoardVal[13][13];

	// info changing every submove
	struct MoveInfo {
		// the current part of the active tower that is still moved with
		// This stays equal during split selection and only changes after the split happens
		uint8_t currentSelection = 0;
		int currentX = -1;
		int currentY = -1;

		uint8_t stepsMade = 0;

		// Amount of pieces to continue move with
		int split = 0;

		bool isSplitting = false;

		// Does the selection still wield the blue power
		// Stays true during captures with blue
		bool blueProperty = false;
		bool standingOnTurnPiece = false;

		// True while in a capture (*on* enemy pieces)
		bool isCapturing = false;
		// True when merged
		bool isMerging = false;
		// True when selection lands on an empty or friendly occupied field
		bool hasCaptured = false;

		MoveInfo() {};
	} moveInfo;

	std::vector<std::pair<uint8_t(*)[13], MoveInfo>> prevStates;

	void updateDisplay();
	void resetMove(const bool print);
	void endMove();

	void moveInDirection(Direction dir);
	void turn();
	
	void changeSplitSelection(const int amountToMoveWith);
	void changeTowerSelection(const int x, const int y);

	void switchPushMergeMode();

	bool isValdMoveToEnd() const;
};

