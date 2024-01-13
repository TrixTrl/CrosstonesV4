#pragma once
#include "Globals/Player.h"
#include <Windows.h>
#include <WinUser.h>
#include <stack>

class ManualPlayer : public Player
{
public:
	ManualPlayer(HWND _globalHwnd, uint8_t(*_displayBoard)[13][13]) 
		: originalDisplayBoardVal(), moveInfo(), prevStateStack()
	{
		globalHwnd = _globalHwnd;
		targetBoard = nullptr;
		displayBoard = _displayBoard;
	}

	void getMoveToPlay(uint8_t(*board)[13][13], bool isWhite, int endTime);
	void keyDown(const PlayerInputKey key);

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
	uint8_t maxSteps = 0;
	// the *active* tower in it's beginning state
	uint8_t startTower = 0;

	//the board to which the final changes should be made.
	uint8_t(*targetBoard)[13][13];

	//access to display to show submoves
	uint8_t(*displayBoard)[13][13];
	HWND globalHwnd;

	//copy of display before modifying it
	uint8_t originalDisplayBoardVal[13][13];

	// info changing every submove
	struct MoveInfo {
		// the current part of the active tower that is still moved with
		// This stays equal during split selection and only changes after the split happens
		uint8_t currentSelection = 0;
		uint8_t currentX = 0;
		uint8_t currentY = 0;

		uint8_t stepsMade = 0;

		// Amount of pieces to leave behind
		uint8_t splitSelection = 0;

		// Does the selection still wield the blue power
		// Stays true during captures with blue
		bool blueProperty = false;
		bool standingOnTurnPiece = false;

		// True while in a capture (*on* enemy pieces)
		bool isCapturing = false;
		// True when selection lands on an empty or friendly occupied field
		bool hasCaptured = false;

		MoveInfo() {};
	} moveInfo;

	std::stack<std::pair<uint8_t[13][13], MoveInfo>> prevStateStack;

	void moveInDirection(Direction dir);

	void updateDisplay();
	void resetMove(bool print);
	void changeSplitSelection(const uint8_t amountToMoveWith);
	void changeTowerSelection(const uint8_t x, const uint8_t y);

	bool isValdMoveToEnd() const;
	uint8_t getMaxSteps(const uint8_t tower) const;
};

